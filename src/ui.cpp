#include "ui.hpp"
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Pack.H>
#include <Fl/Fl_Progress.H>
#include <math.h>
#include <fstream>
#include <iostream>
#include "formats.hpp"
#include "tree_view.hpp"
#include "treemap_widget.hpp"

UIState g_ui_state;

const double SCAN_TIMER_TICK = 0.03;

Fl_Box* add_stat_row(const char* key_label, const char* value_label) {
  auto* row = new Fl_Flex(0, 0, 0, TEXT_ROW_HEIGHT, Fl_Flex::HORIZONTAL);

  auto* key = new Fl_Box(0, 0, 0, TEXT_ROW_HEIGHT, key_label);
  key->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  key->labelfont(MAIN_FONT);

  auto* value = new Fl_Box(0, 0, 0, TEXT_ROW_HEIGHT, value_label);
  value->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
  value->labelfont(MAIN_FONT_BOLD);

  row->end();
  return value;
}

void flex_spacer(Fl_Flex* flex_element, int size) {
  auto* spacer = new Fl_Box(0, 0, 0, 0);
  flex_element->fixed(spacer, size);
}

void update_stat_labels(int drive_idx) {
  if (drive_idx < 0 || drive_idx >= (int)drives.size())
    return;
  const DriveInfo& drive = drives[drive_idx];

  uint64_t used_bytes = drive.total_size - drive.free_size;

  static char buf_total[32];
  static char buf_used[64];
  static char buf_free[64];
  char size_part[32];
  char percent_part[32];

  g_ui_state.val_selected->label(drive.name.c_str());

  get_size_string(drive.total_size, buf_total, 32);
  g_ui_state.val_total->label(buf_total);

  get_size_string(used_bytes, size_part, 32);
  get_size_percent_string(used_bytes, drive.total_size, percent_part, 32);
  snprintf(buf_used, 64, "%s (%s)", size_part, percent_part);
  g_ui_state.val_used->label(buf_used);

  get_size_string(drive.free_size, size_part, 32);
  get_size_percent_string(drive.free_size, drive.total_size, percent_part, 32);
  snprintf(buf_free, 64, "%s (%s)", size_part, percent_part);
  g_ui_state.val_free->label(buf_free);

  g_ui_state.val_selected->redraw();
  g_ui_state.val_total->redraw();
  g_ui_state.val_used->redraw();
  g_ui_state.val_free->redraw();
}

void drive_choice_cb(Fl_Widget* widget, void* data) {
  auto* choice = (Fl_Choice*)widget;

  int selected_idx = choice->value();
  update_stat_labels(selected_idx);
}

void update_ui_timer_cb(void* data) {
  auto* ctx = (ScanContext*)data;

  if (ctx->is_finished.load()) {
    g_ui_state.progress_bar->value(100.0);
    g_ui_state.progress_bar->redraw();

    if (g_ui_state.scan_thread.joinable()) {
      g_ui_state.scan_thread.join();
    }

    g_ui_state.root = ctx->root_node;
    g_ui_state.tree_view->fill_flat_view(ctx->root_node);
    g_ui_state.tree_map->set_root(ctx->root_node);

    g_ui_state.is_scanning = false;
    g_ui_state.current_ctx = nullptr;
    delete ctx;
    return;
  }

  const float P = 0.4F;
  const float K = 100.0F;

  float n_pow = std::powf((float)(ctx->files_scanned.load()), P);
  float progress_val = 100.0F * (n_pow / (n_pow + K));

  g_ui_state.progress_bar->value(progress_val);
  g_ui_state.progress_bar->redraw();

  Fl::repeat_timeout(SCAN_TIMER_TICK, update_ui_timer_cb, ctx);
}

void background_scan_worker(ScanContext* ctx) {
  iterate_dir(ctx->start_path, ctx->root_node, ctx);
  sort_directory_tree(ctx->root_node);
  ctx->is_finished = true;
  Fl::awake();
}

void analyze_button_cb(Fl_Widget* widget, void* data) {
  if (g_ui_state.is_scanning)
    return;

  int idx = g_ui_state.drive_choice->value();

  std::string path_to_scan;

  if (idx == drives.size()) {
    Fl_Native_File_Chooser fnfc;
    fnfc.title("Choose a directory");
    fnfc.type(Fl_Native_File_Chooser::BROWSE_DIRECTORY);

    if (fnfc.show() != 0)
      return;
    path_to_scan = fnfc.filename();
  } else {
    path_to_scan = drives[idx].name;
  }

  if (path_to_scan.empty())
    return;

  if (file_tree_buffer_size > 0) {
    memset(file_tree_buffer, 0, file_tree_buffer_size * sizeof(FileNode));

    size_t used_words = (file_tree_buffer_size >> BIT_SHIFT) + 1;
    size_t bytes_to_clear = used_words * sizeof(uint64_t);

    memset(is_directory_mask, 0, bytes_to_clear);
    memset(is_expanded_mask, 0, bytes_to_clear);
  }
  file_tree_buffer_size = 0;
  arena_free_all(string_arena);
  g_ui_state.root = nullptr;
  g_ui_state.tree_map->set_root(nullptr);
  g_ui_state.tree_view->fill_flat_view(nullptr);

  size_t bytes_needed = path_to_scan.size() + 1;
  char* arena_path =
      (char*)arena_alloc_align(string_arena, bytes_needed, DEFAULT_ALIGNMENT);
  if (arena_path == nullptr)
    return;

  memcpy(arena_path, path_to_scan.c_str(), bytes_needed);

  auto* ctx = new ScanContext();
  ctx->start_path = arena_path;
  ctx->root_node = add_root_node(ctx->start_path);

  g_ui_state.current_ctx = ctx;
  g_ui_state.is_scanning = true;
  g_ui_state.scan_thread = std::thread(background_scan_worker, ctx);

  Fl::add_timeout(SCAN_TIMER_TICK, update_ui_timer_cb, ctx);
}

Fl_Flex* analyze_section() {
  auto* pack = new Fl_Flex(0, 0, 0, 0, Fl_Flex::VERTICAL);
  pack->margin(MARGIN);

  auto* row = new Fl_Flex(0, 0, 0, 0, Fl_Flex::HORIZONTAL);

  auto* choice_label = new Fl_Box(0, 0, 0, 0, "Choose: ");
  choice_label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  choice_label->labelfont(MAIN_FONT);

  auto* choice = new Fl_Choice(0, 0, 0, 0);
  g_ui_state.drive_choice = choice;
  choice->textfont(MAIN_FONT);
  for (const DriveInfo& drive : drives) {
    choice->add(drive.name.c_str());
  }
  choice->add("<Select folder>");
  choice->value(0);
  choice->visible_focus(0);
  choice->callback(drive_choice_cb);

  flex_spacer(row, MARGIN);

  auto* analyze = new Fl_Button(0, 0, 0, 0, "Analyze");
  analyze->labelfont(MAIN_FONT);
  analyze->box(FL_THIN_UP_BOX);
  analyze->visible_focus(0);
  analyze->callback(analyze_button_cb);

  row->end();

  flex_spacer(pack, ANALYZER_SECTION_ROW_GAP);

  auto* progress_bar = new Fl_Progress(0, 0, 0, 0);
  g_ui_state.progress_bar = progress_bar;
  progress_bar->box(FL_BORDER_BOX);
  progress_bar->color(FL_WHITE, PROGRESSBAR_COLOR);
  progress_bar->minimum(0.);
  progress_bar->maximum(100.);
  progress_bar->value(0.);

  pack->end();
  return pack;
}

Fl_Flex* stat_section() {
  auto* pack = new Fl_Flex(0, 0, 0, 0);
  pack->margin(MARGIN);
  g_ui_state.val_selected = add_stat_row("Selected: ", "");
  g_ui_state.val_total = add_stat_row("Total Volume: ", "");
  g_ui_state.val_used = add_stat_row("Used: ", "");
  g_ui_state.val_free = add_stat_row("Free: ", "");
  pack->end();

  if (!drives.empty()) {
    update_stat_labels(0);
  }

  return pack;
}

void export_txt_cb(Fl_Widget* w, void* data) {
  Fl_Native_File_Chooser file_chooser(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);

  file_chooser.title("Export Analysis Results");
  file_chooser.preset_file("export.txt");
  file_chooser.filter("Text Files\t*.txt");

  file_chooser.options(Fl_Native_File_Chooser::SAVEAS_CONFIRM);

  if (file_chooser.show() != 0) {
    return;
  }

  std::string filepath = file_chooser.filename();

  if (filepath.size() < 4 || filepath.substr(filepath.size() - 4) != ".txt") {
    filepath += ".txt";
  }

  std::ofstream outfile(filepath);
  if (!outfile.is_open()) {
    return;
  }

  traverse_tree_out(g_ui_state.root, 0, outfile);
  outfile.close();
}

void export_csv_cb(
    Fl_Widget* w,
    void* data) {  // TODO(IlyaBelykh): refactor to avoid repetition
  Fl_Native_File_Chooser file_chooser(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);

  file_chooser.title("Export Analysis Results");
  file_chooser.preset_file("export.csv");
  file_chooser.filter("Comma-Separated Values\t*.csv");

  file_chooser.options(Fl_Native_File_Chooser::SAVEAS_CONFIRM);

  if (file_chooser.show() != 0) {
    return;
  }

  std::string filepath = file_chooser.filename();

  if (filepath.size() < 4 || filepath.substr(filepath.size() - 4) != ".csv") {
    filepath += ".csv";
  }

  std::ofstream outfile(filepath);
  if (!outfile.is_open()) {
    return;
  }

  outfile << "Level 0;Level 1;Level 2;Level 3;Level 4;Level 5;Level 6;Level "
             "7;Level 8;Level 9;Level 10;Level 11;Type;Size "
             "(bytes);Size;Percentage of parent\n";
  traverse_tree_csv(g_ui_state.root, 0, outfile);
  outfile.close();
}

Fl_Flex* main_div() {
  auto* main_layout = new Fl_Flex(0, 0, DEFAULT_WINDOW_WIDTH,
                                  DEFAULT_WINDOW_HEIGHT, Fl_Flex::VERTICAL);

  auto* menu_bar = new Fl_Menu_Bar(0, 0, 0, 0);
  menu_bar->add("File/Export as TXT...", 0, export_txt_cb);
  menu_bar->add("File/Export as CSV...", 0, export_csv_cb);
  menu_bar->box(FL_FLAT_BOX);
  main_layout->fixed(menu_bar, MENU_BAR_SIZE);

  auto* top_row = new Fl_Flex(0, 0, 0, 0, Fl_Flex::HORIZONTAL);

  Fl_Flex* analyze_container = analyze_section();
  new Fl_Box(0, 0, 0, 0);
  Fl_Flex* stat_container = stat_section();

  top_row->fixed(analyze_container, INFO_CONTAINER_SIZE);
  top_row->fixed(stat_container, INFO_CONTAINER_SIZE);
  top_row->end();

  main_layout->fixed(top_row, TOP_ROW_HEIGHT);

  auto* middle_row = new Fl_Flex(0, 0, 0, 0, Fl_Flex::HORIZONTAL);

  auto* tree_view = new TreeView(0, 0, 0, 0);
  tree_view->set_font(MAIN_FONT, FONT_SIZE);
  g_ui_state.tree_view = tree_view;

  middle_row->end();

  auto* treemap_widget = new FileTreeMap(0, 0, 0, 0);
  g_ui_state.tree_map = treemap_widget;

  main_layout->fixed(treemap_widget, TREEMAP_WIDGET_SIZE);

  main_layout->end();

  return main_layout;
}