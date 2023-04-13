/*
2D CAD modules to make it easier to specify 2D shapes
for manual cutting and drilling guides (export to DXF then use
inkscape to render as svg and print)

As well as shared stuff between all the cutting guides

Author: Phil McCarthy Dec 2020
*/

// globals
top_board_width = 200;
top_board_height = 130;

module rounded_rectangle( width, length, radius_corner ) {
	translate( [ radius_corner, radius_corner, 0 ] )
		minkowski() {
			square([width - 2 * radius_corner, length - 2 * radius_corner]);
			circle( radius_corner );
		}
}

module offset_circle(c, d) {
    translate(c)
        circle(d=d);
}

module circle_group( centres, diam, pt_offset=[0,0]) {
    for(centre = centres) {
        translate(pt_offset)
            offset_circle(centre, diam);
    }
}

module support_rods_and_bracket_holes(draw_outer, pt_offset=[0,0]) {
    support_rod_diameter = 12;
    support_rod_holes = [[20,20], [180,20], [20,110], [180,110]];
    bracket_offset = 18.5 + (support_rod_diameter/2);
    inner_mounting_bracket_holes = [[20+bracket_offset,20], [180-bracket_offset,20], [20+bracket_offset,110], [180-bracket_offset,110]];
    outer_mounting_bracket_holes = [[20-bracket_offset,20], [180+bracket_offset,20], [20-bracket_offset,110], [180+bracket_offset,110]];
    circle_group( support_rod_holes, support_rod_diameter, pt_offset);
    circle_group( inner_mounting_bracket_holes, 4, pt_offset);
    if (draw_outer) {
        circle_group( outer_mounting_bracket_holes, 4, pt_offset);
    }
}