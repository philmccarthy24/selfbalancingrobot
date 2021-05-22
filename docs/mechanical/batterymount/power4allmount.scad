include <power4all.scad>

difference() {
union() {
    // battery mounts
    translate ([-65, 0, 0]) power4all();
    translate ([65, 0, 0]) power4all();
    // left/right screw mounts
    translate([80, -25, 0]) cylinder(r=10, h=2);
    translate([-80, -25, 0]) cylinder(r=10, h=2);
    // connectors to main cable channel
    translate ([-81, 58, 0]) cube([32, 10, 20]);
    translate ([49, 58, 0]) cube([32, 10, 20]);
    
    difference() {
        // main cable channel shape
        intersection() {
            topboard();
            cablerun();
        }
        // minus cable run space
        hull() {
            translate ([-75, 82, 0]) rotate([0, 90, 0]) cylinder(r=7.5, h=150);
            translate ([-75, 82, 10]) rotate([0, 90, 0]) cylinder(r=7.5, h=150);
        };
        // mnus switch box interior volume
        translate ([-25, 42, 0]) switchboxinterior(50,50,50);
    };
    
   difference() { 
        translate ([-25, 42, 0])
        difference() {
            union() {
                difference() {
                    union() {
                        difference() {
                            // main switch box
                            switchbox(50, 50, 50);
                            // minus interior volume
                            switchboxinterior(50, 50, 50);
                            // minus rocker switch hole
                            translate([25, 35, 35]) rotate([0, 50, 90]) cylinder(r=10, h=5);
                            
                        }
                        // cylindrical support for center screw mount
                        intersection() {
                            switchbox(50, 50, 50);
                            translate([25, 0, 0]) rotate([0, 0, 90]) cylinder(r=12.5, h=50);
                        }
                    }
                    // minus center screw pad space
                    translate([25, 0, 2]) rotate([0, 0, 90]) cylinder(r=10, h=50);
                    // minus DC cable egress hole
                    translate([25, 2, 25]) rotate([0, 100, 90]) cylinder(r=2.5, h=20);
                }
                // center screw mount pad
                translate([25, 0, 0]) rotate([0, 0, 90]) cylinder(r=10, h=2);
            }
            // center screw hole
            translate([25, 0, -1]) rotate([0, 0, 90]) cylinder(r=2, h=4);
        }
        // take away interior cable volume again
        hull() {
            translate ([-75, 82, 0]) rotate([0, 90, 0]) cylinder(r=7.5, h=150);
            translate ([-75, 82, 10]) rotate([0, 90, 0]) cylinder(r=7.5, h=150);
        };
    }
}
// interior cable holes to left/right mounts from main channel
translate ([-71, 55, 8]) cube([16, 25, 9]);
translate ([58, 55, 8]) cube([16, 25, 9]);
// screw holes for top-left/right mounts
translate([-80, -25, -1]) cylinder(r=2, h=5);
translate([80, -25, -1]) cylinder(r=2, h=5);
// main cable egress point
translate ([70, 82, 12]) rotate([0, 90, 0]) cylinder(r=5, h=40);

}

module switchbox(width, length, height, sideradius=10, topradius=5, backradius=5) {
    hull() {
        translate([0, length-10, 0]) cube([width, 10, 10]);
        translate([0, 20, height]) rotate([0, 90, 0]) cylinder(r=topradius, h=width);
        translate([0, length-backradius, 20]) rotate([0, 90, 0]) cylinder(r=backradius, h=width);
        translate([5, 0, 0]) cube([width-10, 5, 1]);
        translate([-5, length - (length/3), 0]) cylinder(r=sideradius, h=20);
        translate([width+5, length - (length/3), 0]) cylinder(r=sideradius, h=20);
    }   
}

module switchboxinterior(width, length, height) {
    translate([2.5,2.5,2.5])
    union() {
      scale([.9,.9,.9]) switchbox(50,50,50);
      translate([0,0,-10]) scale([.9,.9,.9]) switchbox(width, length, height);
    }
}

module cablerun() {
        translate([100,68,20])
        rotate([-90,0,90])
        linear_extrude(height=200) {
            hull() {
                translate ([20, 0, 0]) circle(r=5);
                translate ([0, 0, 0]) square([25,20]);
            }
        }  
}

// original top board, to act as a cutout for main cable run
module topboard() {
    translate([-(100 - 30), 30 + (93 - 130), 0]) 
    linear_extrude(height=40) {
    minkowski() {
			square([200 - 2 * 30, 130 - 2 * 30]);
			circle( 30 );
		}
    };
}