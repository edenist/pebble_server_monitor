var initialised = false;

var ip = '';
var port = '8080';
var update_interval = -1;
var auto_update = -1;

function fetchServerStats(ip, FIELD) {
  var response;
  var req = new XMLHttpRequest();
  // build the GET request
  console.log("connecting to: " + ip);
  req.TimeOut = 2000;
  req.open('GET', "http://" + ip + ":" + port + "/pebbleTest.json", true);
  req.onload = function(e) {
    console.log('1');
    if (req.readyState == 4) {
      console.log('2');
      //200 - HTTP OK
      if(req.status == 200) {
        console.log('3');
        console.log(req.responseText);
        response = JSON.parse(req.responseText);
        var cpu;
        var mem;
        var host;
        if (response.Data) {
          cpu = response.Data.cpu;
          mem = response.Data.mem;
          host = response.Data.host;
          switch(FIELD) {
            case 'all':
              console.log('ALL');
              Pebble.sendAppMessage({
                "mem": mem.toString(), "cpu": cpu.toString(), "host": host.toString()});
            break;
            case 'host':
              console.log(host);
              Pebble.sendAppMessage({
                "host": host.toString()});
              break;
            case 'cpu':
              console.log(cpu);
              Pebble.sendAppMessage({
                "cpu": cpu.toString()});
              break;
            case 'mem':
              console.log(mem);
              Pebble.sendAppMessage({
                "mem": mem.toString()});
              break;
            default:
              console.log('Invalid FIELD passed to fetchServerStats function');
          }
        }
      } else {
        console.log("Request returned error code " + req.status.toString());
      }
    } else {
      console.log("Failed to get JSON object from server at ");
    }
  }
  req.ontimeout = function() {
    console.log("Failed to get JSON object from server at ");
  }
  req.ontimeout = function(f) {
    console.log('timeout');
  }
  req.onabort = function(f) {
    console.log('error');
  }
  console.log('4');
  req.send(null);
  console.log('5');
}
  
// Callback for app ready event
Pebble.addEventListener("ready", 
  function(e) {
    console.log("Connected!" + e.ready);

    ip = localStorage.getItem("ip");
    if (!ip) {
      ip = '192.168.1.9';
    }
    
    port = localStorage.getItem("port");
    if (!port) {
      port = '8080';
    }

    update_interval = localStorage.getItem("update_interval");
    console.log('JS - update_interval: ' + update_interval.toString());
    if (update_interval == -1) {
      update_interval = 1000;
    }
    console.log('JS - update_interval: ' + update_interval.toString());

    auto_update = localStorage.getItem("auto_update");
    console.log('JS - auto_update: ' + auto_update.toString());
    if (auto_update == -1) {
      auto_update = 1;
    }
    console.log('JS - auto_update: ' + auto_update.toString());

    initialised = true;
    console.log(e.type);
  }
);

// Callback for phone app configuration window
Pebble.addEventListener("showConfiguration", function() {
  console.log("showing configuration");
  Pebble.openURL('http://root.robotprofessor.net/pebble_server_monitor_config.php?' +
                          "ip=" + ip + 
                          "&port=" + port +
                          "&update_interval=" + (update_interval/1000).toString() +
                          "&auto_update=" + auto_update.toString()
                        );
});

// Callback for saving configuration options
Pebble.addEventListener("webviewclosed", function(e) {
  console.log("configuration closed");
  var options = JSON.parse(decodeURIComponent(e.response));

  ip = options.ip
  localStorage.setItem("ip", ip);

  port = options.port;
  localStorage.setItem("port", port);

  update_interval = parseInt(options.update_interval)*1000;
  localStorage.setItem("update_interval", update_interval);

  auto_update = parseInt(options.auto_update);
  localStorage.setItem("auto_update", auto_update);

  var watchConfig = {"ip": ip, "port": port, "auto": auto_update.toString(), "update_int": update_interval.toString()};

  Pebble.sendAppMessage(watchConfig);
  
  console.log("Options = " + JSON.stringify(options));
  console.log("IP set to " + ip);
  console.log("port set to " + port);
  console.log("update interval ser to " + update_interval.toString());
  console.log("auto_update set to " + auto_update.toString());
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
        console.log("message - FETCH SETTINGS");
        Pebble.sendAppMessage({"ip": ip, "port": port, "auto": auto_update.toString(), "update_int": update_interval.toString()});
        //Pebble.sendAppMessage({"ip": ip});
        //Pebble.sendAppMessage({"auto": auto_update.toString()});
        //Pebble.sendAppMessage({"update_int": update_interval.toString()});
        //fetchServerStats(ip, 'fetch');
      }
      if (e.payload.host) {
        console.log("message - HOST");
        fetchServerStats(ip, 'host');
      }
      if (e.payload.cpu) {
        console.log("message - CPU");
        fetchServerStats(ip, 'cpu');
      }
      if (e.payload.mem) {
        console.log("message - MEM");
        fetchServerStats(ip, 'mem');
      }
      if (e.payload.all) {
        console.log("message - ALL");
        fetchServerStats(ip, 'all');
      }
    }
);


