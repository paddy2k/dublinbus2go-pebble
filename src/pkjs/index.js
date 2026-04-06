console.log("PKJS: Script Start");

var appMessageQueue = {
  queue: [],
  numTries: 0,
  maxTries: 5,
  working: false,
  clear: function () {
    this.queue = [];
    this.working = false;
  },
  isEmpty: function () {
    return this.queue.length === 0;
  },
  nextMessage: function () {
    return this.isEmpty() ? {} : this.queue[0];
  },
  send: function (message) {
    if (message) this.queue.push(message);
    if (this.working) return;
    if (this.queue.length > 0) {
      this.working = true;
      var ack = function () {
        appMessageQueue.numTries = 0;
        appMessageQueue.queue.shift();
        appMessageQueue.working = false;
        appMessageQueue.send();
        console.log(
          "AppMessage SENT: " + JSON.stringify(appMessageQueue.nextMessage()),
        );
      };
      var nack = function () {
        appMessageQueue.numTries++;
        appMessageQueue.working = false;
        appMessageQueue.send();
        console.log(
          "AppMessage NACK: " + JSON.stringify(appMessageQueue.nextMessage()),
        );
      };
      if (appMessageQueue.numTries >= appMessageQueue.maxTries) {
        console.log(
          "Failed sending AppMessage: " +
            JSON.stringify(appMessageQueue.nextMessage()),
        );
        ack();
      }
      console.log(
        "Sending AppMessage: " + JSON.stringify(appMessageQueue.nextMessage()),
      );
      Pebble.sendAppMessage(appMessageQueue.nextMessage(), ack, nack);
    }
  },
};

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
/*  Latitude/longitude spherical geodesy formulae & scripts (c) Chris Veness 2002-2010            */
function LatLon(lat, lon, rad) {
  window["LatLonCache"] = window["LatLonCache"] || {};
  rad = rad || 6371;

  this._lat = lat;
  this._lon = lon;
  this._radius = rad;
}

LatLon.prototype.setCache = function (key, value) {
  window["LatLonCache"][key] = value;
};

LatLon.prototype.getCache = function (key) {
  return window["LatLonCache"][key];
};
LatLon.prototype.distanceTo = function (point, precision) {
  var type = "d";
  var sourceKey = this._lat * (this._lon * 100);
  var targetKey = point._lat * (point._lon * 100);
  var key =
    type +
    (sourceKey < targetKey ? sourceKey + targetKey : targetKey + sourceKey);

  if (!this.getCache(key)) {
    if (typeof precision == "undefined") {
      precision = 7;
    }
    var R = this._radius;
    var lat1 = this._lat.toRad(),
      lon1 = this._lon.toRad();
    var lat2 = point._lat.toRad(),
      lon2 = point._lon.toRad();
    var dLat = lat2 - lat1;
    var dLon = lon2 - lon1;
    var a =
      Math.sin(dLat / 2) * Math.sin(dLat / 2) +
      Math.cos(lat1) * Math.cos(lat2) * Math.sin(dLon / 2) * Math.sin(dLon / 2);
    var c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
    var d = R * c;

    this.setCache(key, d.toPrecisionFixed(precision));
  }
  return this.getCache(key);
};
LatLon.prototype.bearingTo = function (point) {
  var type = "b";
  var sourceKey = this._lat * (this._lon * 100);
  var targetKey = point._lat * (point._lon * 100);
  var key =
    type +
    (sourceKey < targetKey ? sourceKey + targetKey : targetKey + sourceKey);

  if (!this.getCache(key)) {
    var lat1 = this._lat.toRad(),
      lat2 = point._lat.toRad();
    var dLon = (point._lon - this._lon).toRad();
    var y = Math.sin(dLon) * Math.cos(lat2);
    var x =
      Math.cos(lat1) * Math.sin(lat2) -
      Math.sin(lat1) * Math.cos(lat2) * Math.cos(dLon);
    var brng = Math.atan2(y, x);
    this.setCache(key, (brng.toDeg() + 360) % 360);
  }
  return this.getCache(key);
};
if (typeof String.prototype.toRad === "undefined") {
  Number.prototype.toRad = function () {
    return (this * Math.PI) / 180;
  };
}
if (typeof String.prototype.toDeg === "undefined") {
  Number.prototype.toDeg = function () {
    return (this * 180) / Math.PI;
  };
}
if (typeof Number.prototype.toPrecisionFixed === "undefined") {
  Number.prototype.toPrecisionFixed = function (precision) {
    var numb = this < 0 ? -this : this;
    var sign = this < 0 ? "-" : "";
    if (numb == 0) {
      n = "0.";
      while (precision--) {
        n += "0";
      }
      return n;
    }
    var scale = Math.ceil(Math.log(numb) * Math.LOG10E);
    var n = String(Math.round(numb * Math.pow(10, precision - scale)));
    if (scale > 0) {
      l = scale - n.length;
      while (l-- > 0) {
        n = n + "0";
      }
      if (scale < n.length) {
        n = n.slice(0, scale) + "." + n.slice(scale);
      }
    } else {
      while (scale++ < 0) {
        n = "0" + n;
      }
      n = "0." + n;
    }
    return sign + n;
  };
}

var Utils = {
  prettyBearing: function (rawBearing) {
    // Offset bearing by 45 deg
    var offset = 22.5;
    var correctedBearing = (rawBearing + 22.5) % 360;

    if (correctedBearing <= 45) {
      bearing = "N";
    } else if (correctedBearing <= 90) {
      bearing = "NE";
    } else if (correctedBearing <= 135) {
      bearing = "E";
    } else if (correctedBearing <= 180) {
      bearing = "SE";
    } else if (correctedBearing <= 225) {
      bearing = "S";
    } else if (correctedBearing <= 270) {
      bearing = "SW";
    } else if (correctedBearing <= 315) {
      bearing = "W";
    } else if (correctedBearing <= 360) {
      bearing = "NW";
    }

    return bearing;
  },

  testPrettyBearing: function (deg, bearing) {
    console.log("Prettifing " + deg + "°, expects " + bearing);

    var prettyBaring = Utils.prettyBearing(deg);
    var result = prettyBaring == bearing ? "PASS" : "FAIL";
    console.log("Result: " + result);
  },

  prettyDistance: function (rawDistance) {
    var distance;

    if (rawDistance < 1) {
      distance = Math.floor(rawDistance * 1000) + "m";
    } else {
      distance = Math.round(rawDistance * 100) / 100 + "km";
    }

    return distance;
  },
};

var results = {
  min: 10,
  max: 20,
};

var actions = {
  getSavedStops: 0,
  getNearestStops: 1,
  getStop: 2,
  getStops: 3,
  saveStop: 4,
  removeStop: 5,
  showUi: 6,
  getNearestBus: 7,
  getNearestTrain: 8,
  getNearestTram: 9,
};

var sendStatus = {
  start: 0,
  inProgress: 1,
  end: 2,
  error: 3,
};

var db2go = {
  baseDomain:
    "https://ifp3fy4xcgnzusjiogkvj5hqwa0hnudb.lambda-url.eu-west-1.on.aws",
};

db2go.geoLookupUrl = db2go.baseDomain + "/stops";
db2go.stopsDetailsUrl = db2go.baseDomain + "/departures?stop_id=8230DB007443";

db2go.getXML = function (url, data, callback) {
  var xmlhttp = new XMLHttpRequest();
  var data = data || "";
  var callback = callback || function () {};

  xmlhttp.open("POST", url, true);
  xmlhttp.onreadystatechange = function () {
    if (xmlhttp.readyState == 4) {
      if (xmlhttp.status == 200) {
        var responseXML = xmlhttp.responseXML;
        var responseRaw = xmlhttp.responseText;
        callback.call(this, responseXML, responseRaw);
      }
    }
  };

  // Send the POST request
  xmlhttp.setRequestHeader("Content-Type", "text/xml");
  return xmlhttp.send(data);
};

db2go.getJSON = function (url, callback) {
  var xmlhttp = new XMLHttpRequest();
  var callback = callback || function () {};

  xmlhttp.open("GET", url, true);
  xmlhttp.onreadystatechange = function () {
    if (xmlhttp.readyState == 4) {
      if (xmlhttp.status == 200) {
        var response = JSON.parse(xmlhttp.responseText);
        callback.call(this, response);
      }
    }
  };

  return xmlhttp.send();
};

db2go.getStop = function (stopId, callback) {
  console.log("Get Stop:", stopId);
  var requestUrl = db2go.baseDomain + "/departures?stop_id=" + stopId;
  db2go.getJSON(requestUrl, function (departures) {
    callback.call(this, departures);
  });
};

db2go.getStops = function (callback, type, stopI) {
  type = type || "";
  console.log("getStops");
  var latDiff = 0.003;
  var lngDiff = 0.005;

  var stopI = (stopI * stopI) / 2 + 1 || 1;

  // var lat = 53.28029602383617;
  // var lng = -6.306788024878215;

  var lat = db2goLocation.coords.latitude;
  var lng = db2goLocation.coords.longitude;

  // var neCoords = lat + stopI * latDiff + "," + (lng + stopI * lngDiff);
  // var swCoords = lat - stopI * latDiff + "," + (lng - stopI * lngDiff);
  var requestUrl =
    db2go.geoLookupUrl + "?lat=" + lat + "&lon=" + lng + "&type=" + type;

  console.log("Request URL:", requestUrl);
  db2go.getJSON(requestUrl, function (stops) {
    if (stops.length < results.min && stopI < 10) {
      console.log("More stops needed: " + stops.length);
      db2go.getStops(callback, type, stopI);
    } else {
      console.log("Stops found:", stops.length);
      callback.call(this, stops);
    }
  });
};

db2go.listStops = function (stops) {
  console.log("List Stops", stops.length);
  var here = new LatLon(
    db2goLocation.coords.latitude,
    db2goLocation.coords.longitude,
  );

  console.log(stops.length + " stop nearby");
  var message = {
    action: actions.getStops,
    status: sendStatus.end,
  };

  if (!stops.length) {
    message["status"] = sendStatus.error;
    appMessageQueue.send(message);

    return false;
  }

  stops.forEach(function (stop, index) {
    // var StopLocation = new LatLon(stop.stop_lat, stop.stop_lon);
    var rawDistance = stop.distance_m / 1000;
    // var rawDistance = here.distanceTo(StopLocation);
    // var rawBearing = here.bearingTo(StopLocation);
    var rawBearing = stop.bearing_degrees;
    var distance = Utils.prettyDistance(rawDistance);
    var bearing = Utils.prettyBearing(rawBearing);
    // var nameArray = stop.address.split(",");
    stops[index].rawDistance = rawDistance;
    stops[index].distance = distance;
    stops[index].bearing = bearing;
    // stops[index].name = nameArray[1] || nameArray[0];
    stops[index].name = stop.stop_name;
    // stops[index].id = parseInt(stops[index].stopnumber);
    stops[index].id = stop.stop_id.toString();
  });

  // stops.sort(function (a, b) {
  //   // Sorting
  //   return a.rawDistance - b.rawDistance;
  // });
  stops.splice(results.max);

  stops.forEach(function (stop, index) {
    message["stop_name_" + index] = stop.name;
    message["stop_id_" + index] = stop.id;
    message["stop_distance_" + index] = stop.distance;
    message["stop_bearing_" + index] = stop.bearing;
  });

  appMessageQueue.send(message);
  console.log("LIST STOPS: End");
};

db2go.storedStops = function () {
  var stops = [];
  var stopIds = JSON.parse(localStorage.getItem("stops")) || [];
  stopIds.forEach(function (stopId) {
    var stop = JSON.parse(localStorage.getItem(stopId));
    if (stop) {
      stops.push(stop);
    }
  });

  return stops;
};

db2go.encodeStops = function (stops) {
  var stopsParts = [];

  stops.forEach(function (stop) {
    var routes = [];
    if (stop.routes) {
      stop.routes.split("<br/>").forEach(function (route) {
        routes.push(route.split(" ")[0]);
      });
      routes.splice(-1); // Remove trailing space
    }
    console.log(stop.name);
    console.log(JSON.stringify(routes));
    stop.exclude = stop.exclude || [];
    var stopParts = [
      stop.id,
      encodeURIComponent(stop.name),
      stop.exclude.join("|"),
    ];
    stopsParts.push(stopParts.join("="));
  });

  return stopsParts.join("~");
};

db2go.decodeStops = function (payload) {
  var stops = [];
  var parts = payload.split("~");

  parts.forEach(function (stop) {
    var temp = stop.split("=");
    stops.push({
      id: temp[0],
      name: decodeURIComponent(temp[1]),
      exclude: temp[2].split("|"),
    });
  });

  return stops;
};

db2go.parseStopAndroid = function (departures, stopId) {
  console.log("parseStopAndroid", departures.length);
  var maxResponse = 12;
  // var response = xml.getElementsByTagName("DocumentElement")[0];
  var message = {
    action: actions.getStop,
    status: sendStatus.end,
    id: stopId.toString(),
  };
  console.log("A", departures.length);
  if (departures.length === 0) {
    appMessageQueue.send({
      action: actions.getStop,
      status: sendStatus.error,
    });
    return false;
  }

  console.log("B", departures.length);
  // var stops = response.childNodes;
  var responseSize = departures.length;
  responseSize = responseSize < maxResponse ? responseSize : maxResponse;
  console.log("responseSize", responseSize);
  for (var i = 0; i < responseSize; i++) {
    // var route, destination, now, expected, dueIn;
    // var stop = departures[i];
    console.log(departures[i]);
    var route = departures[i].route_name || "";
    var destination = departures[i].destination || "";
    var dueIn = parseInt(departures[i].due_in);
    if (isNaN(dueIn)) {
      dueIn = 0;
    }

    // Don't send negative values
    if (dueIn < 0) {
      dueIn = 0;
    }
    console.log(i, route, destination, dueIn);

    message["bus_route_" + i] = route;
    message["bus_destination_" + i] = destination;
    message["bus_duein_" + i] = dueIn;

    console.log(route + " " + destination + " - " + dueIn + " mins");
  }
  console.log(JSON.stringify(message));
  appMessageQueue.send(message);
};

db2go.parseStopApple = function (text, a) {
  var maxResponse = 20,
    stopId,
    tmp;
  var message = {
    action: actions.getStop,
    status: sendStatus.end,
  };

  var stops_regex = /(<StopData\s.*?<\/StopData>)/g;
  var route_regex =
    /<MonitoredVehicleJourney_PublishedLineName>(.*?)<\/MonitoredVehicleJourney_PublishedLineName>/;
  var destination_regex =
    /<MonitoredVehicleJourney_DestinationName>(.*?)<\/MonitoredVehicleJourney_DestinationName>/;
  var now_regex = /<Timestamp>(.*?)<\/Timestamp>/;
  var expected_regex =
    /<MonitoredCall_ExpectedArrivalTime>(.*?)<\/MonitoredCall_ExpectedArrivalTime>/;
  var stopid_regex =
    /<MonitoredStopVisit_MonitoringRef>(.*?)<\/MonitoredStopVisit_MonitoringRef>/;

  var stops = [];
  while ((tmp = stops_regex.exec(text)) !== null) {
    if (tmp.index === stops_regex.lastIndex) {
      stops_regex.lastIndex++;
    }
    stops.push(tmp);
  }

  if (!stops.length) {
    appMessageQueue.send({
      action: actions.getStop,
      status: sendStatus.error,
    });
    return false;
  }

  var responseSize = stops.length;
  responseSize = responseSize < maxResponse ? responseSize : maxResponse;

  for (var i = 0; i < responseSize; i++) {
    var route, destination, now, expected, dueIn;
    var stop = stops[i];

    var route_matches = route_regex.exec(stop);
    route = route_matches[1];

    var destination_matches = destination_regex.exec(stop);
    destination = destination_matches[1].split(" via ")[0];

    var now_matches = now_regex.exec(stop);
    now = new Date(now_matches[1]);

    var expected_matches = expected_regex.exec(stop);
    expected = new Date(expected_matches[1]);

    var stopid_matches = stopid_regex.exec(stop);
    stopId = parseInt(stopid_matches[1]);

    dueIn = Math.round((expected - now) / 60 / 1000);

    // Don't send negative values
    if (dueIn < 0) {
      dueIn = 0;
    }

    message["bus_route_" + i] = route;
    message["bus_destination_" + i] = destination;
    message["bus_duein_" + i] = dueIn;
    message["id"] = stopId;

    console.log(route + " " + destination + " - " + dueIn + " mins");
  }

  console.log(JSON.stringify(message));
  appMessageQueue.send(message);
};

var db2goLocation = {
  coords: false,
  options: {
    enableHighAccuracy: false,
    maximumAge: 30000,
    timeout: 5000,
  },
  success: function (pos) {
    db2goLocation.coords = pos.coords;

    console.log(
      "Watch at ... : " +
        db2goLocation.coords.latitude +
        "," +
        db2goLocation.coords.longitude,
    );
  },
  error: function (err) {
    console.log("location error (" + err.code + "): " + err.message);
    appMessageQueue.send({
      action: actions.getStops,
      status: sendStatus.error,
    });
  },
  get: function (callback) {
    navigator.geolocation.getCurrentPosition(
      function (pos) {
        db2goLocation.success.call(this, pos);
        callback.call();
      },
      db2goLocation.error,
      db2goLocation.options,
    );
  },
};

Pebble.addEventListener("ready", function (e) {
  console.log("PKJS: Pebble Ready Event Fired");

  // Kick-off GeoLocation after ready event to ensure safer execution context
  db2goLocation.get(function () {
    console.log("1st Fix");
  });

  // Request current position with a small delay to ensure watch is ready
  setTimeout(function () {
    appMessageQueue.send({
      action: actions.showUi,
      status: sendStatus.end,
    });
  }, 500);

  console.log("Cold Start Request");
  db2go.getStops(function (err, stops) {
    return false;
  });
});

Pebble.addEventListener("appmessage", function (e) {
  var message = e.payload;
  console.log("Message: " + message.action);
  console.log(JSON.stringify(message));

  switch (message.action) {
    case actions.getSavedStops:
      console.log("Get Saved Stops: Start");
      var stops = db2go.storedStops();
      db2goLocation.get(function () {
        db2go.listStops(stops);
      });

      break;
    case actions.removeStop:
      var stopIds = JSON.parse(localStorage.getItem("stops")) || [];
      var tempIds = [];
      stopIds.forEach(function (stopId) {
        if (message.id != stopId) {
          tempIds.push(stopId);
        }
      });

      appMessageQueue.send({
        action: actions.removeStop,
        status: sendStatus.end,
      });
      localStorage.setItem("stops", JSON.stringify(tempIds));
      console.log("Stop Removed: " + stopIds);
      break;

    case actions.saveStop:
      var stops =
        window["stops"] || JSON.parse(localStorage.getItem("stopsCache"));
      var stopIds = JSON.parse(localStorage.getItem("stops")) || [];
      stops.forEach(function (stop) {
        if (message.id == stop.id) {
          if (stopIds.indexOf(message.id) === -1) {
            stopIds.push(message.id);
            localStorage.setItem("stops", JSON.stringify(stopIds));
            localStorage.setItem(message.id, JSON.stringify(stop));
          }

          appMessageQueue.send({
            action: actions.saveStop,
            status: sendStatus.end,
          });
          console.log("Stop Saved: " + stopIds);
        }
      });
      break;

    case actions.getNearestStops:
      console.log("Get Nearest Stops: Start OK");
      db2goLocation.get(function () {
        db2go.getStops(function (stops) {
          console.log("getStops Inner", stops.length);
          window["stops"] = stops;
          db2go.listStops(stops);

          setTimeout(function () {
            localStorage.setItem("stopsCache", JSON.stringify(stops));
          }, 0);
        }, "bus");
      });
      break;

    case actions.getNearestBus:
      console.log("Get Nearest Bus: Start");
      db2goLocation.get(function () {
        db2go.getStops(function (stops) {
          window["stops"] = stops;
          db2go.listStops(stops);
          setTimeout(function () {
            localStorage.setItem("stopsCache", JSON.stringify(stops));
          }, 0);
        }, "bus");
      });
      break;

    case actions.getNearestTrain:
      console.log("Get Nearest Train: Start");
      db2goLocation.get(function () {
        db2go.getStops(function (stops) {
          window["stops"] = stops;
          db2go.listStops(stops);
          setTimeout(function () {
            localStorage.setItem("stopsCache", JSON.stringify(stops));
          }, 0);
        }, "train");
      });
      break;

    case actions.getNearestTram:
      console.log("Get Nearest Tram: Start");
      db2goLocation.get(function () {
        db2go.getStops(function (stops) {
          window["stops"] = stops;
          db2go.listStops(stops);
          setTimeout(function () {
            localStorage.setItem("stopsCache", JSON.stringify(stops));
          }, 0);
        }, "luas");
      });
      break;

    case actions.getStop:
      var stopId = message.id.toString();

      console.log("Get Stop: " + stopId);
      db2go.getStop(stopId, function (departures) {
        console.log("Got Stop: ", departures);
        db2go.parseStopAndroid(departures, stopId);
        // if (!xml) {
        //   db2go.parseStopApple(text, a);
        // } else {
        //   db2go.parseStopAndroid(xml, a);
        // }
      });
      break;
  }
});

Pebble.addEventListener("showConfiguration", function (e) {
  var stops = db2go.storedStops();
  var message = db2go.encodeStops(stops);
  var configPage = "http://192.168.1.7:8080/";

  console.log("Show Configuration");
  Pebble.openURL(configPage + "#" + message);
});

Pebble.addEventListener("webviewclosed", function (e) {
  var configuration = JSON.parse(decodeURIComponent(e.response));
  console.log("Configuration window returned: ", JSON.stringify(configuration));
});

console.log("PKJS: Script initialization complete");
