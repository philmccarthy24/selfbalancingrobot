use <threadlib/threadlib.scad>

$fn=100;
// battery holder 78 across
// 21 wide
// 21 high
// spokes need 2⌀ circle 2mm in from batt ends
// TP module is 17.25 x 28 (looku holder from scepter proj).
// mount vertically? (as there are no supports etc)
// 

// rocker switch 12 x 19 

//LM2596 module dims 21.25 x 43.25 x 14 clearance height
// screw holes at 6.5 in and 2.5 in, at each end  

//translate([0,19,1])  tp4056_holder();
enclosure();
//translate([25,4,15]) LM2596_footprint();

module tp4056_holder()
{
    // 22 length so pads stick out
    // 1.5 pcb thickness
    translate([0,10,0]) mirror([0,1,0]){
        cube([22, 8.5, 2]);
        cube([22, 10, 1.5]);
        translate([0,0,1.5+17.25]) cube([22,10,1.5]);
        translate([0,0,1.5+17.25-0.5])cube([22,8.5,2]);
        difference() {
            cube([15,5,17.25+3]);
            translate([2,2,2]) cube([15,5,17.25-1]);
        }
    }
}

module enclosure()
{
    difference() {
        union() {
            translate([-2,-2,0]) cube([82,54,40]);
            // lid screw pillars
            translate([-2,-2,0]) cylinder(h=40, d=10);
            translate([80,-2,0]) cylinder(h=40, d=10);
            translate([-2,52,0]) cylinder(h=40, d=10);
            translate([80,52,0]) cylinder(h=40, d=10);
        }
        translate([0,0,2]) cube([78,50,42]);
        // indent for tp module glueing
        translate([0,18-0.1,1]) cube([22+0.1,10+0.2,2]);
        // hole for rocker switch
        translate([78-1,15,10]) cube([5,12,19]);
        // hole for mini usb charging port 3x8
        translate([-3,18.25-3,1+1.5+((17.25/2)-(8/2))]) cube([5,3,8]);
        // power out hole
        translate([12,-3,20]) rotate([-90,0,0]) cylinder(h=4,d=5);
        // lid pillar screw holes
        translate([-2,-2,25]) tap("M3", turns=30);
        translate([80,-2,25]) tap("M3", turns=30);
        translate([-2,52,25]) tap("M3", turns=30);
        translate([80,52,25]) tap("M3", turns=30);
    }
    // raised battery holder subfloor with runs for cable
    translate([0,50-21,2])
    difference() {
        cube([78,21,5]);
        translate([0,(21/2)-(3/2),0]) cube([40,3,6]);
        translate([45,(21/2)-(3/2),0]) cube([45,3,6]);
        translate([40-3,-1,0]) cube([3,21/2,6]);
        translate([45,-1,0]) cube([3,21/2,6]);
    }
    // standoffs for LM2596
    translate([25+6.5-(6/2),0,0]) difference() {
        hull() {
            cube([6,0.1,0.1]);
            translate([6/2,0,15+2.5]) rotate([-90,0,0]) cylinder(h=4,d1=6,d2=5);
        };
        translate([6/2,1,15+2.5]) rotate([-90,0,0]) tap("M3", turns=8);
    }
    
    translate([25+43.25-6.5-(6/2),0,0]) difference() {
        hull() {
            translate([0,0,20]) cube([6,0.1,0.1]);
            translate([6/2,0,15+21.25-2.5]) rotate([-90,0,0]) cylinder(h=4,d1=6,d2=5);
        };
        translate([6/2,1,15+21.25-2.5]) rotate([-90,0,0]) tap("M3", turns=8);
    }
    
}

module LM2596_footprint()
{
    difference() {
        cube([43.25,1.5,21.25]);
        translate([43.25-6.5,-1,21.25-2.5]) rotate([-90,0,0]) cylinder(h=3, d=2.9);
        translate([6.5,-1,2.5]) rotate([-90,0,0]) cylinder(h=3, d=2.9);
    }
}