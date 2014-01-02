var background = false;
var showday = false;     
var initialized = false; // set true when pebble sends default values
var configReq = false; // set true when we have a request to open the configuration page
var log = false;

Pebble.addEventListener("showConfiguration", function() {
    if (initialized) {
        showConfigPage();
    } else {
        configReq = true;
        requestValues();
    } 
    requestValues();                                                  
});                                   

function showConfigPage() {
    if (log) console.log("Opening config page w/ background="+background+" and dom="+showday);
    configReq = false;
    // time param is just to avoid caching issues
    var uri = "http://appling.org/pebble/bigtimedate/configure.html?time"+new Date().getTime()
    			+"background="+background+"&dom="+showday;
    Pebble.openURL(uri);
}
        
Pebble.addEventListener("appmessage",
  function(e) {
	if (log) console.log(" JS received message: "+ JSON.stringify(e.payload));
    if (e.payload.hasOwnProperty("background")) {
        background = !!e.payload.background;
    }
    if (e.payload.hasOwnProperty("showday")) {
        showday = !!e.payload.showday;
    }
    if (log) console.log("JS now background="+background+", showday="+showday );
    initialized = true;
    if (configReq) {
        window.setTimeout(showConfigPage, 0); // let listener return so that sender won't get error
    }
  }
);

Pebble.addEventListener("webviewclosed", function(e) {
	if (log) console.log("JS configuration closed");
  if (e.response && e.response.length > 0) {
     var options = JSON.parse(decodeURIComponent(e.response));
     if (log) console.log("JS Options = " + JSON.stringify(options));
     for (var index in options) {
         var next = options[index];
         if (next.name == "background") {
             background = next.value == "true";
         } else if (next.name == "dom") {
             showday = next.value == "true";
         }
     }
     sendToPebble();
 }
});


function requestValues() {
	if (log) console.log("JS requesting initial values");
    Pebble.sendAppMessage({"reqvalues": 1});
}

function sendToPebble() {    
    var msg = { 
        "background" : (background ? 1 : 0),
        "showday"    : (showday ? 1 : 0)
    };
    if (log) console.log("JS msg to send:"+JSON.stringify(msg));
    Pebble.sendAppMessage(msg);
}