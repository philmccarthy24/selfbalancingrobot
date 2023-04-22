$fn=100;

use <threadlib/threadlib.scad>

//powerbank_mount();

difference() {
    translate([-84/2,174/2-58/2,70]) breadboard_mount(width=58,length=169,height=10,thickness=2);
    // m3 bolt holes
    translate([-5,174/8*3,69]) cylinder(h=6, d=3);
    translate([-5,174/8*5,69]) cylinder(h=6, d=3);
    translate([84+5,174/8*3,69]) cylinder(h=6, d=3);
    translate([84+5,174/8*5,69]) cylinder(h=6, d=3);
    translate([-5,174/8*3,72]) cylinder(h=6, d=6);
    translate([-5,174/8*5,72]) cylinder(h=6, d=6);
    translate([84+5,174/8*3,72]) cylinder(h=6, d=6);
    translate([84+5,174/8*5,72]) cylinder(h=6, d=6);
}

difference() {
    union() {
        translate([-5,174/8*3,70]) cylinder(h=10, d1=20, d2=10);
        translate([-5,174/8*5,70]) cylinder(h=10, d1=20, d2=10);
        translate([84+5,174/8*3,70]) cylinder(h=10, d1=20, d2=10);
        translate([84+5,174/8*5,70]) cylinder(h=10, d1=20, d2=10);
    }
    translate([-84/2,174/2-58/2,70]) cube([169,58,11]);   
}

module powerbank_mount() {
    difference() {
        union() {
            cube([80+4, 170+4, 45]);
            translate([-5,174/8*3,0]) cylinder(h=70, d=20);
            translate([-5,174/8*5,0]) cylinder(h=70, d=20);
            translate([84+5,174/8*3,0]) cylinder(h=70, d=20);
            translate([84+5,174/8*5,0]) cylinder(h=70, d=20);
        }
        translate([2,2,2]) cube([80, 170, 70]);
        translate([15+2,-1,2]) cube([80-30, 180, 45]);
        
        translate([-5,174/8*3,50]) tap("M3", turns=40);
        translate([-5,174/8*5,50]) tap("M3", turns=40);
        translate([84+5,174/8*3,50]) tap("M3", turns=40);
        translate([84+5,174/8*5,50]) tap("M3", turns=40);
        
        translate([20,20,-0.1]) cylinder(h=2.2, d1=4,d2=7);
        translate([84-20,20,-0.1]) cylinder(h=2.2, d1=4,d2=7);
        translate([20,174-20,-0.1]) cylinder(h=2.2, d1=4,d2=7);
        translate([84-20,174-20,-0.1]) cylinder(h=2.2, d1=4,d2=7);
    }
}

// cribbed from another project
module breadboard_mount(width,length,height,thickness) {
    translate([length/2,width/2,0])
    rotate([0,0,90]) {
        translate([-width/2,-length/2,0]) cube([width,length,5]);
        translate([0,0,5])
        difference() {
            union() {
                difference() {
                    translate([-width/2,-length/2,0]) cube([width,length,height]);
                    // vertical volume cutout
                    translate([0,0,15]) cube([width-(thickness*2),length-(thickness*2), 50], true);
                    // top slice
                    translate([-width,-length/2+thickness,height]) cube([width*2,length-(thickness*2), 10]);
                    
                    // breadboard notch cutouts
                    translate([-width/2-thickness, length/2-thickness-12-5, -0.5]) cube([5,5,7]);
                    translate([-width/2-thickness, -5/2, -0.5]) cube([5,5,7]);
                    translate([-width/2-thickness, -length/2+thickness+12, -0.5]) cube([5,5,7]);
                }
                
                // breadboard mounting notches
                translate([width/2-thickness-1.5, length/2-thickness-12-4-1, 0]) cube([1.5,3.5,5]);
                translate([width/2-thickness-1.5, -4/2, 0]) cube([1.5,3.5,5]);
                translate([width/2-thickness-1.5, -length/2+thickness+12+1, 0]) cube([1.5,3.5,5]);
                  
            }
        }
    }
}