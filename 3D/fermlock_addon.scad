$fn=128;

color("red", 0.75)
ring();

color("blue", 0.75)
translate([0,0,30])
sensorHolder();

color("green", 0.75)
translate([0,0,50])
cakeStick();


color("pink", 0.75)
translate([0,0,15])
cakeStickAttachment();

color("violet", 0.75)
translate([15.5,0,38.5])
rotate([0,-90,0])
sensor();


color("yellow", 0.75)
translate([-22,0,36])
rotate([0,0,90])
cubeCell();


module cubeCell() {
    cube([41.5, 25, 7.6], center=true);
}


module sensor() {
difference() {
    cube([32.5,19.5,1.5], center = true);
    
    translate([-8.5,0,0])
    cylinder(d=3.5, h=2, center=true);
    
}
translate([12.8,0,5.3])
cube([7, 11, 12], center = true);

translate([-13.5,0,1.4])
cube([3, 17, 2], center = true);

translate([1,3,1.4])
cube([8, 6, 2], center = true);    

}

//4.2 diameter easy flow
//4.0 stick to filament
module cakeStickAttachment() {
    cylinder(d=19, h=0.5, center=true);
    
    translate([0,0,2.5])
    difference() {
        cylinder(d=7,h=5, center=true);        
        cylinder(d=3.8,h=5.1, center=true);
    }
}



//15 x 0.4 x 0.4
module cakeStick() {
    cylinder(d=4.0,h=50, center=true);
}

module sensorHolder() {
    difference() {
    union() {
        handle();
        
        //90deg rotated and spaced
        spacedHandle();
            
        translate([0,0,13])
        difference() {
            cylinder(d=17,h=28, center=true);
            cylinder(d=4.3,h=28.1, center=true);
            translate([6,0,8.5])
            sphere(d=10);
        }
        
        translate([10,0,0.5])
        difference() {
            cube([15,2,3], center=true);
            translate([5.5,0,1.1])
            cube([10,2.1,1], center=true);
        }
        
        difference() {
            translate([9,0,6])
            cube([12,16,14], center=true);
            
            translate([15.8,0,8.75])
            rotate([0,-90,0])
            sensor();
            
        }

        
        //cable holder 2x
        translate([0,-12.5,4])
        cableHolder();
        translate([0,12.5,4])
        cableHolder();
        
        //antenna holder
        translate([0,-34,4])
        cableHolder();
        
        
        //cable holder on the side for temp sensor
        translate([46.5,15.5,0.5])
        rotate([0,0,0])
        difference() {
            cylinder(d=10,h=3, center=true);      
            cylinder(d=5, h=3.1, center=true);
        }
     }
    
    cylinder(d=4.3,h=4.1, center=true);        
         
    }

}

module cableHolder() {
    
    rotate([0,90,0])
    difference() {
        cylinder(d=10,h=2, center=true);      
        cylinder(d=5, h=2.1, center=true);
    }
  
    
}


module spacedHandle() {

    rotate([0,0,90])
    difference () {
            minkowski() {
                cube([25,86,2], center=true);
                cylinder(r=2,h=1);
            }
            
            minkowski() {
                cube([17,72.5,2.1], center=true);
                cylinder(r=2,h=1);
            }
                 
            translate([0,41.5,0])
                cylinder(d=4.8,h=4.1, center=true);
            
            translate([0,-41.5,0])
                cylinder(d=4.8,h=4.1, center=true);
          }   


}

module handle() {
    difference () {
            minkowski() {
                cube([5,86,2], center=true);
                cylinder(r=2,h=1);
            }
                    
            translate([0,41.5,0])
                cylinder(d=4.8,h=4.1, center=true);
            
            translate([0,-41.5,0])
                cylinder(d=4.8,h=4.1, center=true);
          }    

}

module ring() {
    difference() {
        cylinder(h=2, r=33.25, center=true);
        cylinder(h=2.1, r=30.50, center=true);
    }
   
    translate([0,-33.5,0])
        stick();
    
    translate([0,33.5,0])
        rotate([0,0,180])    
            stick();
    
    translate([33.5,0,0])
        rotate([0,0,90])    
            stick();

    translate([-33.5,0,0])
        rotate([0,0,-90])    
            stick();    
    

}

module stick() {
    translate([0,-3,0])
    cube([4.5,10,2], center=true);
    
    translate([0,-8,14])
    cylinder(h=30, r=2.25, center=true);
    
    translate([-4.9,0,0])
    rotate([0,0,180])
    roundEdge();
    
    translate([4.9,0,0])
    rotate([0,180,180])
    roundEdge();
    
    
    translate([0,-4.5,5])
    rotate([0,-90,0])
    scale([1.5,0.5,0.5])
    roundEdge();
    
    
    translate([0,-8,23])
    sphere(d=5);


}

module roundEdge() {
    difference() {
        cylinder(r=10, h=2, center=true);

        translate([8,8,0])
            cylinder(r=11, h=2.1, center=true);

        translate([-7.7,0,0])
        cube([10,20,2.1], center=true);
        
        translate([0,-7.7,0])
        cube([20,10,2.1], center=true);
    }
}




