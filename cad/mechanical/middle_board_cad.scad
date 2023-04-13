// globals
board_width_extent = 295+140;
board_height_extent = 250;

include <common.scad>

top_board_pt_offset = [(board_width_extent/2) - (top_board_width/2),(board_height_extent/2)-(top_board_height/2)];

difference() {
    union() {
        translate([0,20])
            rounded_rectangle(board_width_extent, board_height_extent*0.7, 50);
        
        middle_section_width = board_width_extent*0.75;
        translate([(board_width_extent/2)-(middle_section_width/2),0])
            rounded_rectangle(middle_section_width, board_height_extent, 50);
    }
    
    union() {
        support_rods_and_bracket_holes(true, top_board_pt_offset);
        
        // cable and data hole
        translate([20,top_board_height-50])
            offset_circle(top_board_pt_offset, 20);
    }
}