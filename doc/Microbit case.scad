use <D2Lib v2.scad>;

// Dimensions in mm
// Ender3 v2: x=224, y=212
// battery box: 15.5 x 25.8 x 52.75 conn: 5.6 x 4.2
// microbit: 43.2 x 11 x 51.8
// cable: 60 x 7.2 x 66     16.4x7.2   10x6.2
//
// L: 1.2 + 3x (44.2 + .6 + 26.8) + .6 + 1.2 - .6 = 218.4
// W: 1.2 + 9x (7.2 + .6 + 15.5) + .6 + 1.2 - .6 = 216.9

inner_height = 50;
outer_wall = 1.2;
inner_wall = 0.6;
bottom = 1;
battery_l = 26.8;
battery_w = 16;
connb_w = 6.5;
connb_l = 6.5;
usbm_w = 7;
usbm_l = 15;
usba_w = 10;
usba_l = 17.4;
microbit_l = 44.2;

count_w = 9;
count_l = 3;

inner_width = usbm_w + inner_wall + battery_w;
inner_length = microbit_l + inner_wall + battery_l;


// Init
$fn=$preview ? 20 : 50;
q = .01;
q2 = q*2;

total_inner_width = (inner_width + inner_wall) * count_w - inner_wall;
total_inner_length = (inner_length + inner_wall) * count_l - inner_wall;

echo("Total x: ", total_inner_width + outer_wall * 2);
echo("Total y: ", total_inner_length + outer_wall * 2);

// bottom
beam_xyz(0, total_inner_width + outer_wall * 2, 0, total_inner_length + outer_wall * 2, 0, bottom);

// outer wall
beam_xyz(0,total_inner_width + outer_wall * 2, 0, outer_wall, bottom, inner_height + bottom);
beam_xyz(0,total_inner_width + outer_wall * 2, total_inner_length + outer_wall, total_inner_length + outer_wall * 2, bottom, inner_height + bottom);
beam_xyz(0, outer_wall, 0, total_inner_length + outer_wall * 2, bottom, inner_height + bottom);
beam_xyz(total_inner_width + outer_wall, total_inner_width + outer_wall * 2, 0, total_inner_length + outer_wall * 2, bottom, inner_height + bottom);

// inner walls
for (x = [1:count_w-1])
    beam_xyz(outer_wall + x * (inner_width + inner_wall) - inner_wall, outer_wall + x * (inner_width + inner_wall), outer_wall, total_inner_length + outer_wall, bottom, inner_height + bottom);

for (y = [1:count_l-1])
    beam_xyz(outer_wall,outer_wall + total_inner_width, outer_wall + y * (inner_length + inner_wall) - inner_wall, outer_wall + y * (inner_length + inner_wall), bottom, inner_height + bottom);

// inner compartiments
for (x = [0:count_w-1])
    for (y = [0:count_l-1])
        translate([x * (inner_width + inner_wall) + outer_wall, y * (inner_length + inner_wall) + outer_wall, bottom])
            innerWalls();

module innerWallsV1()
{
    // h walls
    beam_xyz(0,inner_width, microbit_l, microbit_l + inner_wall, 0, inner_height);
    beam_xyz(0,usba_w, usba_l, usba_l + inner_wall, 0, inner_height);
    beam_xyz(0,usba_w, microbit_l - usba_w - inner_wall, microbit_l - usba_w, 0, inner_height);
    beam_xyz(0,usbm_w, microbit_l + inner_wall + usbm_w, microbit_l + inner_wall + usbm_w + inner_wall, 0, inner_height);
    beam_xyz(0,usbm_w, inner_length - usbm_l - inner_wall, inner_length - usbm_l, 0, inner_height);
    
    // v walls
    beam_xyz(usba_w, usba_w + inner_wall, 0, microbit_l, 0, inner_height);
    beam_xyz(usbm_w, usbm_w + inner_wall, microbit_l + inner_wall, inner_length, 0, inner_height);
}


module innerWalls()
{
    // h walls
    beam_xyz(0,inner_width, microbit_l, microbit_l + inner_wall, 0, inner_height);
    beam_xyz(0,usba_w, usba_l, usba_l + inner_wall, 0, inner_height);
    beam_xyz(usba_w + inner_wall, usba_w + inner_wall + connb_w + inner_wall, connb_l, connb_l + inner_wall, 0, inner_height);
    beam_xyz(0,usbm_w, inner_length - usbm_l - inner_wall, inner_length - usbm_l, 0, inner_height);
    
    // v walls
    beam_xyz(usba_w, usba_w + inner_wall, 0, microbit_l, 0, inner_height);
    beam_xyz(usbm_w, usbm_w + inner_wall, microbit_l + inner_wall, inner_length, 0, inner_height);
    beam_xyz(usba_w + inner_wall + connb_w, usba_w + inner_wall + connb_w + inner_wall, 0, connb_l + inner_wall, 0, inner_height);
}