var initialised = false;

var ip = localStorage.getItem("ip");

if (!ip) {
  ip = "192.168.1.9";
}

function fetchServerStats(ip) {
  var response;
  var req = new XMLHttpRequest();
  // build the GET request
  req.open('GET', "http://" + ip + "/pebbleTest.json", true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      //200 - HTTP OK
      if(req.status == 200) {
        console.log(req.responseText);
        response = JSON.parse(req.responseText);
        var cpu;
        var mem;
        var host;
        if (response.Data) {
          cpu = response.Data.cpu;
          mem = response.Data.mem;
          host = response.Data.host;
          console.log(cpu);
          console.log(mem);
          console.log(host);
          Pebble.sendAppMessage({
            "cpu": cpu.toString()});
          Pebble.sendAppMessage({
            "mem": mem.toString()});
          Pebble.sendAppMessage({
            "host": host.toString()});
        }
      } else {
        console.log("Request returned error code " + req.status.toString());
      }
    }
  }
  req.send(null);
}
  
// Callback for app ready event
Pebble.addEventListener("ready",
    function(e) {
        console.log("Connected!" + e.ready);
        initialised = true;
        console.log(e.type);
    }
);

// Callback for phone app configuration window
Pebble.addEventListener("showConfiguration", function() {
  console.log("showing configuration");
  Pebble.openURL('http://192.168.1.7/configurable.html');
});

// Callback for saving configuration options
Pebble.addEventListener("webviewclosed", function(e) {
  console.log("configuration closed");
  var options = JSON.parse(decodeURIComponent(e.response));
  console.log("Options = " + JSON.stringify(options));
});

// Callback for appmessage events
Pebble.addEventListener("appmessage",
    function(e) {
      console.log("message");
      //if (e.payload.ip) {
      //  console.log("message - IP");
      //  ip = e.payload.ip;
      //  localStorage.setItem("ip", ip);
      //  fetchServerStats(ip);
      //}
      if (e.payload.fetch) {
        console.log("message - FETCH");
        Pebble.sendAppMessage({"ip": ip});
        fetchServerStats(ip);
      }
      //if (e.payload.cpu) {          //only needed for single items being refreshed?
      //  console.log("message - CPU");
      //  fetchServerStats(ip);
      //}
    }
);


