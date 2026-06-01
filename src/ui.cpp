#include "ui.hpp"
#include "tree_view.hpp"
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>
#include <Fl/Fl_Progress.H>

UIState g_ui_state;

Fl_Box* add_stat_row(const char* key_label, const char* value_label) {
    Fl_Flex* row = new Fl_Flex(0, 0, 0, TEXT_ROW_HEIGHT, Fl_Flex::HORIZONTAL);

    Fl_Box* key = new Fl_Box(0, 0, 0, TEXT_ROW_HEIGHT, key_label);
    key->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    key->labelfont(MAIN_FONT);

    Fl_Box* value = new Fl_Box(0, 0, 0,  TEXT_ROW_HEIGHT, value_label);
    value->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
    value->labelfont(MAIN_FONT_BOLD);

    row->end();
    return value;
}

void flex_spacer(Fl_Flex* flex_element, int size) {
    Fl_Box* spacer = new Fl_Box(0, 0, 0, 0);
    flex_element->fixed(spacer, size);
} 

void update_stat_labels(int drive_idx) {
    if (drive_idx < 0 || drive_idx >= (int)drives.size()) return;
    const DriveInfo& drive = drives[drive_idx];

    uint64_t used_bytes = drive.total_size - drive.free_size;

    static char buf_total[32];
    static char buf_used[64];
    static char buf_free[64];
    char size_part[32];
    char percent_part[32];

    g_ui_state.val_selected->label(drive.name);

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
    Fl_Choice* choice = (Fl_Choice*)(widget);
    
    int selected_idx = choice->value();
    update_stat_labels(selected_idx);
}

Fl_Flex* analyze_section() {
    Fl_Flex* pack = new Fl_Flex(0, 0, 0, 0, Fl_Flex::VERTICAL);
    pack->margin(MARGIN);

    Fl_Flex* row = new Fl_Flex(0, 0, 0, 0, Fl_Flex::HORIZONTAL);

    Fl_Box* choice_label = new Fl_Box(0, 0, 0, 0, "Choose: ");
    choice_label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    choice_label->labelfont(MAIN_FONT);

    Fl_Choice* choice = new Fl_Choice(0, 0, 0, 0);
    g_ui_state.drive_choice = choice;
    choice->textfont(MAIN_FONT);
    for (const DriveInfo& drive : drives) {
        choice->add(drive.name);
    }
    choice->value(0);
    choice->visible_focus(0);
    choice->callback(drive_choice_cb);

    flex_spacer(row, MARGIN);

    Fl_Button* analyze = new Fl_Button(0, 0, 0, 0, "Analyze");
    analyze->labelfont(MAIN_FONT);
    analyze->box(FL_THIN_UP_BOX);
    analyze->visible_focus(0);

    row->end();

    flex_spacer(pack, ANALYZER_SECTION_ROW_GAP);

    Fl_Progress* progress_bar = new Fl_Progress(0, 0, 0, 0);
    progress_bar->box(FL_BORDER_BOX);
    progress_bar->color(FL_WHITE, PROGRESSBAR_COLOR);
    progress_bar->minimum(0.);
    progress_bar->maximum(100.);
    progress_bar->value(66.7);

    pack->end();
    return pack;
}

Fl_Flex* stat_section() {
    Fl_Flex* pack = new Fl_Flex(0, 0, 0, 0);
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

Fl_Flex* main_div(FileNode* root) {
    Fl_Flex* main_layout = new Fl_Flex(0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, Fl_Flex::VERTICAL);
    Fl_Flex* top_row = new Fl_Flex(0, 0, 0, 0, Fl_Flex::HORIZONTAL);

    Fl_Flex* analyze_container = analyze_section();
    new Fl_Box(0, 0, 0, 0);
    Fl_Flex* stat_container = stat_section();

    top_row->fixed(analyze_container, INFO_CONTAINER_SIZE);
    top_row->fixed(stat_container, INFO_CONTAINER_SIZE);
    top_row->end();

    main_layout->fixed(top_row, TOP_ROW_HEIGHT);

    Fl_Flex* middle_row = new Fl_Flex(0, 0, 0, 0, Fl_Flex::HORIZONTAL);

    TreeView* tree_view = new TreeView(0, 0, 0, 0);
    tree_view->set_font(MAIN_FONT, FONT_SIZE);
    tree_view->fill_flat_view(root);

    middle_row->end();

    Fl_Box* treemap_widget = new Fl_Box(0, 0, 0, 0); //TODO(IlyaBelykh): Placeholder element has to be replaced with a real widget
    treemap_widget->box(FL_FLAT_BOX);
    treemap_widget->color(fl_rgb_color(128, 128, 128));

    main_layout->fixed(treemap_widget, TREEMAP_WIDGET_SIZE);

    main_layout->end();

    return main_layout;
}