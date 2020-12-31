include <common.scad>

difference() {
    union() {
        rounded_rectangle(top_board_width, top_board_height, 30);
    }
    
    union() {
        support_rods_and_bracket_holes(false);
    }
}