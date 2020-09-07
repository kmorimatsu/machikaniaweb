/************************************
* MachiKania web written by Katsumi *
*      This script is released      *
*        under the LGPL v2.1.       *
************************************/

// Initialize system
button.key4button.checked=true;
hexfile.load();
system.init();
display.init(system.pFontData,system.pFontData2);
display.set_videomode(VMODE_PWEBGRPH,system.pVRAM);
display.all();
system.reset('PACKMAN.HEX');
// Show display every 40 msec (25 frames/sec)
display.show(40);
// Set HTML title
document.getElementsByTagName("TITLE")[0].innerHTML=hexfile.name+' ver '+system.version;