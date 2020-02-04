/************************************
* MachiKania web written by Katsumi *
*      This script is released      *
*        under the LGPL v2.1.       *
************************************/

get=new Object();
get.query="";
get.init=function(url){
	url=""+url;
	var pos=url.indexOf("?")+1;
	if (pos==0) return;
	this.query=url.substr(pos);
	var data=this.query.split("&");
	var i;
	for (i=0;i<data.length;i++) {
		pos=data[i].indexOf("=");
		if (pos<1) continue;
		this[data[i].substr(0,pos)]=data[i].substr(pos+1);
	}
}
get.init(location.href);
