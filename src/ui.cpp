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

#define MAIN_FONT 16
#define MAIN_FONT_BOLD 17 

Fl_Box* add_stat_row(int width, const char* key_label, const char* value_label) {
    Fl_Group* row = new Fl_Group(0, 0, width, 25);

    Fl_Box* key = new Fl_Box(0, 0, width/2, 25, key_label);
    key->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    key->labelfont(MAIN_FONT);

    Fl_Box* value = new Fl_Box(width/2, 0, width/2,  25, value_label);
    value->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
    value->labelfont(MAIN_FONT_BOLD);

    row->end();
    return value;
}

void flex_spacer(Fl_Flex* flex_element, int size) {
    Fl_Box* spacer = new Fl_Box(0, 0, 0, 0);
    flex_element->fixed(spacer, size);
} 

Fl_Flex* analyze_section() {
    Fl_Flex* pack = new Fl_Flex(0, 0, 300, 300, Fl_Flex::VERTICAL);
    pack->margin(10, 10, 10, 10);

    Fl_Flex* row = new Fl_Flex(0, 0, 0, 0, Fl_Flex::HORIZONTAL);

    Fl_Box* choice_label = new Fl_Box(0, 0, 0, 0, "Choose: ");
    choice_label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    choice_label->labelfont(MAIN_FONT);
    
    flex_spacer(row, 60);

    Fl_Choice* choice = new Fl_Choice(0, 0, 0, 0);
    choice->textfont(MAIN_FONT);
    choice->add("Drive C:");
    choice->add("Drive D:");
    choice->add("Drive E:");
    choice->value(0);

    flex_spacer(row, 10);

    Fl_Button* analyze = new Fl_Button(0, 0, 0, 0, "Analyze");
    analyze->labelfont(MAIN_FONT);
    analyze->box(FL_BORDER_BOX);

    row->end();

    flex_spacer(pack, 30);

    Fl_Progress* progress_bar = new Fl_Progress(0, 0, 0, 0);
    progress_bar->box(FL_BORDER_BOX);
    progress_bar->color(FL_WHITE, fl_rgb_color(92, 237, 115));
    progress_bar->minimum(0.);
    progress_bar->maximum(100.);
    progress_bar->value(66.7);

    pack->end();
    return pack;
}

Fl_Flex* stat_section() {
    Fl_Flex* pack = new Fl_Flex(0, 0, 300, 300);
    pack->margin(10, 10, 10, 10);
    add_stat_row(280, "Selected: ", "Drive C:");
    add_stat_row(280, "Total Volume: ", "930.7 GB");
    add_stat_row(280, "Used: ", "264.3 GB (28.4 %)");
    add_stat_row(280, "Free: ", "666.4 GB (71.6 %)");
    pack->end();
    return pack;
}

Fl_Flex* main_div() {
    Fl::set_font(MAIN_FONT, "Segoe UI");
    Fl::set_font(MAIN_FONT_BOLD, "BSegoe UI");

    Fl_Flex* main_layout = new Fl_Flex(0, 0, 1280, 800, Fl_Flex::VERTICAL);
    Fl_Flex* top_row = new Fl_Flex(0, 0, 0, 0, Fl_Flex::HORIZONTAL);

    Fl_Flex* analyze_container = analyze_section();
    new Fl_Box(0, 0, 0, 0);
    Fl_Flex* stat_container = stat_section();

    top_row->fixed(analyze_container, 500);
    top_row->fixed(stat_container, 500);
    top_row->end();

    main_layout->fixed(top_row, 100); //TODO(IlyaBelykh): UI sizes belong as macro constants at the top 

    Fl_Flex* middle_row = new Fl_Flex(0, 0, 0, 0, Fl_Flex::HORIZONTAL);

    TreeView* tree_view = new TreeView(0, 0, 0, 0);
    tree_view->set_font(MAIN_FONT, 16);

    middle_row->end();

    Fl_Box* treemap_widget = new Fl_Box(0, 0, 0, 0); //TODO(IlyaBelykh): Placeholder element has to be replaced with a real widget
    treemap_widget->box(FL_FLAT_BOX);
    treemap_widget->color(fl_rgb_color(128, 128, 128));

    main_layout->fixed(treemap_widget, 400);

    main_layout->end();

    return main_layout;
}