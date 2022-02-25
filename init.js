/************************************
* MachiKania web written by Katsumi *
*      This script is released      *
*        under the LGPL v2.1.       *
************************************/

// Initialize system
hexfile.load();
system.init();
mmc.setCard();
if (get.ini) {
	var t=get.ini.toUpperCase();
	switch(t){
		case "WIDTH36":
		case "WIDTH48":
		case "WIDTH80":
			filesystem.root["MACHIKAM.INI"]=t;
			break;
		default:
			break;
	}
}
display.init(system.pFontData,system.pFontData2);
display.all();
if (get.debug=='hex') {
	system.reset('DUMMY.HEX');
} else {
	system.reset('MACHIKAM.HEX');
}
// Show display every 40 msec (25 frames/sec)
display.show(40);
