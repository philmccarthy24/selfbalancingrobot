// globals
board_width = 295;
board_height = 150;

include <common.scad>

difference() {
    union() {
        rounded_rectangle(board_width, board_height, 30);
    }
    
    union() {
        stator_axle_plate_width = 40;
        stator_axle_plate_height = 44;
        axle_plate_bolt_holes = [[7.5, 6.5], [32, 6.5], [7.5, 36.5], [32, 36.5]];
        // left plate bolt holes
        // to secure stator axle, will need M8 bolts at least 55mm shaft length (not including head. head diameter must be no more than 12mm)
        circle_group( axle_plate_bolt_holes, 8, [0, (board_height/2) - (stator_axle_plate_height/2)]);
        
        // left cable hole
        offset_circle([stator_axle_plate_width + 20, board_height/2], 20);
       
        // odrive mounting holes
        odrive_width = 140.5;
        odrive_height = 50;
        odrive_left_edge = stator_axle_plate_width + 20 + 20;
        odrive_standoff_mounting_holes = [[2.5,4], [2.5,47.5], [55,5.5], [55,29.1], [93.5,5.5], [93.5,29.1], [138,4], [138,47.5]];
        circle_group( odrive_standoff_mounting_holes, 6, [odrive_left_edge, (board_height/2) - (odrive_height/2) + 15]);
        
        // support rods and mounting bracket holes
        top_board_pt_offset = [(board_width/2) - (top_board_width/2),board_height-top_board_height-5];
        support_rods_and_bracket_holes(true, top_board_pt_offset);
        
        // right cable hole
        offset_circle([odrive_left_edge + odrive_width + 15, board_height/2], 20);
        
        // right plate bolt holes
        circle_group( axle_plate_bolt_holes, 8, [board_width-stator_axle_plate_width, (board_height/2) - (stator_axle_plate_height/2)]);
    }
}