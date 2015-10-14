w=26;
l=55;
h=20;

module oval(w,h, height, center = false) {
 scale([1, h/w, 1]) cylinder(h=height, r=w, center=center,$fn=50);
}

module block(l,w,h){
union(){
	//display matrix 20x20x6, modify h-x to control height
	color("white")translate([20/2+11.5,0,3.2+(h-6)]) cube([20,20,6],center=true);
	//clearance pcb to display
	color("green")translate([l/2,0,(h-5)]) cube([l,w,2],center=true);
	//pcb and battery volume
	color("red") translate([l/2,0,(h-6)/2]) cube ([l,w,h-6], center=true);
}//union
}//block(l,w,h);

module body(){
intersection(){
hull(){
	oval(l*0.7,w*0.7,2,false);
	translate([0,0,10]) hull(){
		translate([-l/2,0,0])oval(l*0.5,w*0.6,5,false);
		translate([l/2,0,0]) cylinder(h=5,r=w/2*1.8, $fn=25);
	}
	translate([0,0,h]) oval(l*0.5,w*0.6,2,false);

}//hull

//make something more interesting with the front and back
rotate(a=-10,v=[0,1,0]) translate([2,0,-20]) cylinder(h=70,r=65/2,$fn=100);

}//intersect
}//translate([l/2,0,-3]) body();

module soften_body(shell=false){
difference(){
 minkowski(){
	sphere(1,$fs=0.5);
	translate([l/2,0,-2]) body();
}
	block(l,w,h);
//we empthy the mass, and leave just outer shell
if(shell==true) { translate([1,0,0.5]) scale(0.95) minkowski(){
	sphere(1,$fs=0.5);
	translate([l/2,0,-2]) body();
	}//mink
	}//if
	}//diff
}//soften_body(false);

//to test the cut seam
zcut=10;
//projection(cut=true) translate([0,0,-zcut]) soften_body(true);

//seam prep
module seam(){
difference(){
 translate([1,0,0.5]) scale(0.95) minkowski(){
	sphere(1,$fs=0.5);
	translate([l/2,0,-2]) body();
	}
 translate([1.6,0,0.5]) scale([0.92,0.9,0.9]) minkowski(){
	sphere(1,$fs=0.5);
	translate([l/2,0,-2]) body();
	}
}//diff
}


module seam_rib(){
translate([0,0,zcut-2]) 
	linear_extrude(height=2) 
		projection(cut=true) translate([0,0,-zcut]) seam();
}

 module seam_inner(){
 translate([0,0,zcut-2]) 
	linear_extrude(height=2) 
		projection(cut=true) translate([0,0,-zcut]) {  
    translate([1,0,0.5]) scale(0.95) minkowski(){
	sphere(1,$fs=0.5);
	translate([l/2,0,-2]) body();
	}
    }
 } // seam_inner();
    
//base, handlebar 25mm radius    
module basegrip(){
difference(){
hull(){
	translate([l/2,0,-3]) oval(l*0.55,w*0.7,2,false);
	translate([l/2,0,-15]) rotate(a=90,v=[1,0,0]) translate([0,0,-10]) cylinder(h=20,r=25/2+2,$fn=25);
}//hull
	translate([l/2,0,-25]) cube([40,40,20],center=true);
	handlebar();
	translate([l/2-25/2-5,0,-25/2]) cube([10,20,2.5],center=true);
	translate([l/2-25/2-3,0,-25/2-1.5]) rotate(a=60,v=[0,1,0]) cube([4,18,2.5],center=true);
	translate([l/2+25/2+5,0,-25/2]) cube([10,20,2.5],center=true);
	translate([l/2+25/2+3,0,-25/2-1.5]) rotate(a=60,v=[0,-1,0]) cube([4,18,2.5],center=true);

}//diff
}//basegrip();


module handlebar(){
	color("blue") translate([l/2,0,-15]) rotate(a=90,v=[1,0,0]) translate([0,0,-50]) cylinder(h=100,r=25/2,$fn=25);
}//handlebar();


module assy(){
union(){
	soften_body();
	translate([0,0,-1]) basegrip();
}
} //assy();


module cut_b(){ //bottom
union(){
	difference(){
		assy();
		translate([0,0,35]) cube([200,100,50],center=true);
		//%seam_rib();
        seam_inner();
		//switch space
		translate([l,0,8]) cube([24,12,4],center=true);
		translate([l,0,8]) cube([10,12,8],center=true);
		translate([l+3,0,8]) cube([1,18,8],center=true);
	}//diff
	
}//union
} 
//translate([0,0,-20])
//cut_b();

module cut_a(){ //top
union(){
    difference(){
            assy();
            //cut_b();
            translate([0,0,-15]) cube([200,100,50],center=true);
            //translate([l,0,5]) cube([12,12,8],center=true);
    }
    seam_rib();
    }//union 
} cut_a();