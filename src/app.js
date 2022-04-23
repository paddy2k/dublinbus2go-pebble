var results = {
  min: 10,
  max: 20
};

var actions = {
  getSavedStops: 0,
  getNearestStops: 1,
  getStop:  2,
  getStops:  3,
  saveStop:  4,
  removeStop:  5,
  showUi:  6,
};

var sendStatus = {
  start: 0,
  inProgress: 1,
  end:  2,
  error: 3
}

var db2go = {
  stopDetailsUrl: 'http://rtpi.dublinbus.ie/DublinBusRTPIService.asmx?op=GetRealTimeStopData',
  geoLookupUrl: 'https://dublinbus.ie/Templates/Public/RoutePlannerService/RTPIMapHandler.ashx?zoom=13&czoom=16&rjson=true',
  
  getXML : function (url, data, callback) {
    var xmlhttp = new XMLHttpRequest();
    var data = data || '';
    var callback = callback || function(){};

    xmlhttp.open('POST', url, true);
    xmlhttp.onreadystatechange = function () {
      if (xmlhttp.readyState == 4) {
        if (xmlhttp.status == 200) {
          var responseXML = xmlhttp.responseXML;
          var responseRaw = xmlhttp.responseText;
          callback.call(this, responseXML,  responseRaw);
        }
      }
    }
    
    // Send the POST request
    xmlhttp.setRequestHeader('Content-Type', 'text/xml');
    return xmlhttp.send(data);
  },
  
  getJSON : function (url, callback) {
    var xmlhttp = new XMLHttpRequest();
    var callback = callback || function(){};

    xmlhttp.open('GET', url, true);
    xmlhttp.onreadystatechange = function () {
      if (xmlhttp.readyState == 4) {
        if (xmlhttp.status == 200) {
          var response = JSON.parse(xmlhttp.responseText);
          callback.call(this, response);
        }
      }
    }

    return xmlhttp.send();
  },
  
  getStop : function(stopId, callback){
    var data =
        '<?xml version="1.0" encoding="utf-8"?>'+
          '<soap:Envelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/">'+
            '<soap:Body>'+
              '<GetRealTimeStopData xmlns="http://dublinbus.ie/">'+
                '<stopId>'+stopId+'</stopId>'+
                '<forceRefresh>true</forceRefresh>'+
              '</GetRealTimeStopData>'+
            '</soap:Body>'+
        '</soap:Envelope>';
    
    return this.getXML(this.stopDetailsUrl, data, callback);
  },
  
  getStops : function(callback, stopI){
    var latDiff = 0.003;
    var lngDiff = 0.005;
    
    var stopI = (stopI * stopI/2)+1 || 1;
  
    var lat = db2goLocation.coords.latitude;
    var lng =  db2goLocation.coords.longitude;
  
    var neCoords = (lat+(stopI * latDiff)) +','+ (lng+(stopI * lngDiff));
    var swCoords = (lat-(stopI * latDiff)) +','+ (lng-(stopI * lngDiff));
    var requestUrl = db2go.geoLookupUrl + '&ne=' + neCoords + '&sw=' + swCoords;
    
    db2go.getJSON(requestUrl, function (stops){
      if(stops.points.length < results.min && stopI < 10){
        console.log('More stops needed: '+ stops.points.length);
        db2go.getStops(callback, stopI);
      }
      else{
        callback.call(this, stops.points);
      }
    });
  },
  listStops: function(stops){    
    var here = new LatLon(db2goLocation.coords.latitude, db2goLocation.coords.longitude);
    
    console.log(stops.length + " stop nearby");
    var message = {
      "action": actions.getStops,
      "status" : sendStatus.end
    };

    if(!stops.length){
      message['status'] = sendStatus.error;
      appMessageQueue.send(message);

      return false;
    }

    stops.forEach(function(stop, index){
      var StopLocation = new LatLon(stop.lat, stop.lng);
      var rawDistance = here.distanceTo(StopLocation);
      var rawBearing = here.bearingTo(StopLocation);
      var distance = Utils.prettyDistance(rawDistance);
      var bearing = Utils.prettyBearing(rawBearing);
      var nameArray = stop.address.split(',');

      stops[index].rawDistance = rawDistance;
      stops[index].distance = distance;
      stops[index].bearing = bearing;
      stops[index].name = nameArray[1] ||  nameArray[0];
      stops[index].id = parseInt(stops[index].stopnumber)
    });

    stops.sort( function(a, b){ // Sorting
      return a.rawDistance - b.rawDistance;
    });
    stops.splice(results.max);

    stops.forEach(function(stop, index){
      message["stop_name_"+index]= stop.name;
      message["stop_id_"+index] = stop.id;
      message["stop_distance_"+index] = stop.distance;
      message["stop_bearing_"+index] = stop.bearing;
    });

    appMessageQueue.send(message);
    console.log("LIST STOPS: End");
  },
  
  storedStops: function(){
    var stops = []; 
    var stopIds = JSON.parse(localStorage.getItem("stops"));
    stopIds.forEach(function(stopId){
      var stop = JSON.parse(localStorage.getItem(stopId));
      stops.push(stop);
    });
    
    return stops;
  },
  encodeStops: function(stops){
  	var stopsParts = [];
  
  	stops.forEach(function(stop){
      var routes = [];
      stop.routes.split('<br/>').forEach(function(route){
        routes.push(route.split(' ')[0]);
      })
      routes.splice(-1); // Remove trailing space
      console.log(stop.name);
      console.log(JSON.stringify(routes));
  		stop.exclude = stop.exclude ||[];
  		var stopParts = [
  			stop.id,
  			encodeURIComponent(stop.name),
  			stop.exclude.join('|')
  		];
  		stopsParts.push(stopParts.join('='));
  	});
  
  	return stopsParts.join('~');
  },
  decodeStops: function (payload){
    var stops = [];
    var parts = payload.split('~');
  
    parts.forEach(function(stop){
      var temp = stop.split('=');
      stops.push({
        id: temp[0],
        name: decodeURIComponent(temp[1]),
        exclude: temp[2].split('|')
      });
    });
  
    return stops;
  },
  parseStopAndroid: function(xml, a){
    var maxResponse = 20;
    var response = xml.getElementsByTagName("DocumentElement")[0];
    var message  = {
      "action": actions.getStop,
      "status" : sendStatus.end
    };

    if(!response){
      appMessageQueue.send({
        "action": actions.getStop,
        "status" : sendStatus.error
      });  
      return false;
    }

    var stops= response.childNodes;
    var responseSize = stops.length;
    responseSize = responseSize < maxResponse ? responseSize : maxResponse;

    for (var i=0;i<responseSize;i++){
      var route, destination, now, expected, dueIn;
      var stop = stops[i].childNodes;

      for (var j=0;j<stop.length;j++){
        switch(stop[j].nodeName){
          case 'MonitoredVehicleJourney_PublishedLineName': 
            route = stop[j].textContent;
            break;
          case 'MonitoredVehicleJourney_DestinationName':
            destination = stop[j].textContent.split(" via ")[0];;
            break;
          case 'Timestamp':
            now = new Date(stop[j].textContent);
            break;
          case 'MonitoredCall_ExpectedArrivalTime':
            expected = new Date(stop[j].textContent);
            break;
          case 'MonitoredStopVisit_MonitoringRef':
            stopId = parseInt(stop[j].textContent);
            break;
        }
      }

      dueIn = Math.round(((expected-now)/60)/1000);

      // Don't send negative values
      if(dueIn < 0){
        dueIn = 0;
      }

      message['bus_route_'+i] = route;
      message['bus_destination_'+i] = destination;
      message['bus_duein_'+i] = dueIn;
      message['id'] = stopId;

      console.log(route +' '+ destination + ' - ' + dueIn + ' mins');
    }
    console.log(JSON.stringify(message));
    appMessageQueue.send(message);
  },
  parseStopApple: function(text, a){
    var maxResponse = 20, stopId, tmp;
    var message  = {
      "action": actions.getStop,
      "status" : sendStatus.end
    };
    
    var stops_regex = /(<StopData\s.*?<\/StopData>)/g; 
    var route_regex = /<MonitoredVehicleJourney_PublishedLineName>(.*?)<\/MonitoredVehicleJourney_PublishedLineName>/; 
    var destination_regex = /<MonitoredVehicleJourney_DestinationName>(.*?)<\/MonitoredVehicleJourney_DestinationName>/; 
    var now_regex = /<Timestamp>(.*?)<\/Timestamp>/; 
    var expected_regex = /<MonitoredCall_ExpectedArrivalTime>(.*?)<\/MonitoredCall_ExpectedArrivalTime>/; 
    var stopid_regex = /<MonitoredStopVisit_MonitoringRef>(.*?)<\/MonitoredStopVisit_MonitoringRef>/; 
    
    var stops = [];
    while ((tmp = stops_regex.exec(text)) !== null) {
      if (tmp.index === stops_regex.lastIndex) {
        stops_regex.lastIndex++;
      }
      stops.push(tmp);
    }
      
    if(!stops.length){
      appMessageQueue.send({
        "action": actions.getStop,
        "status" : sendStatus.error
      });  
      return false;
    }
  
    var responseSize = stops.length;
    responseSize = responseSize < maxResponse ? responseSize : maxResponse;

    for (var i=0;i<responseSize;i++){
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

      dueIn = Math.round(((expected-now)/60)/1000);

      // Don't send negative values
      if(dueIn < 0){
        dueIn = 0;
      }

      message['bus_route_'+i] = route;
      message['bus_destination_'+i] = destination;
      message['bus_duein_'+i] = dueIn;
      message['id'] = stopId;

      console.log(route +' '+ destination + ' - ' + dueIn + ' mins');
    }
    
    console.log(JSON.stringify(message));
    appMessageQueue.send(message);
  }
};

var db2goLocation = {
  coords: false,
  options: {
    enableHighAccuracy: false, 
    maximumAge: 30000, 
    timeout: 5000
  },
  success: function(pos) {
    db2goLocation.coords = pos.coords;
    
    console.log('Watch at: ' + db2goLocation.coords.latitude + ',' + db2goLocation.coords.longitude);
  },
  error: function(err) {
    console.log('location error (' + err.code + '): ' + err.message);
    appMessageQueue.send({
      "action": actions.getStops,
      "status" : sendStatus.error
    });
  },
  get: function(callback){
    navigator.geolocation.getCurrentPosition(
      function(pos){
        db2goLocation.success.call(this, pos);
        callback.call()
      }, 
      db2goLocation.error, 
      db2goLocation.options
    );
  }
};
// Kick-off GeoLocation before the customer needs it
db2goLocation.get(function(){console.log('1st Fix')});

Pebble.addEventListener('ready',
 function(e) {
    // Request current position
    appMessageQueue.send({
      "action" : actions.showUi,
      "status" : sendStatus.end
     }); 
    console.log('Pebble Ready');
 }
);

Pebble.addEventListener("appmessage",
  function(e) {
    var message = e.payload;
    console.log("Message: " + message.action);
    console.log(JSON.stringify(message));

    switch(message.action){
      case actions.getSavedStops:
        console.log("Get Saved Stops: Start");

//         // START DEBUG
//         var message = JSON.parse('{"action":3,"status":2,"stop_name_0":" Watling St","stop_id_0":1997,"stop_distance_0":"295m","stop_bearing_0":"N","stop_name_1":" Watling Street","stop_id_1":1940,"stop_distance_1":"296m","stop_bearing_1":"N","stop_name_2":" Bridgefoot Street","stop_id_2":1939,"stop_distance_2":"344m","stop_bearing_2":"NE","stop_name_3":" Marion Villas","stop_id_3":2379,"stop_distance_3":"347m","stop_bearing_3":"S","stop_name_4":" Bridgefoot Street","stop_id_4":1998,"stop_distance_4":"362m","stop_bearing_4":"NE","stop_name_5":" Ardee St","stop_id_5":2314,"stop_distance_5":"382m","stop_bearing_5":"SE","stop_name_6":" Donore Avenue","stop_id_6":2315,"stop_distance_6":"384m","stop_bearing_6":"S","stop_name_7":" Echlin Street","stop_id_7":1996,"stop_distance_7":"387m","stop_bearing_7":"NW","stop_name_8":" Ardee St","stop_id_8":2380,"stop_distance_8":"400m","stop_bearing_8":"SE","stop_name_9":" Steevens Lane","stop_id_9":1941,"stop_distance_9":"404m","stop_bearing_9":"NW","stop_name_10":" Brabazon Street","stop_id_10":7412,"stop_distance_10":"425m","stop_bearing_10":"E","stop_name_11":" Brabazon Row","stop_id_11":2382,"stop_distance_11":"472m","stop_bearing_11":"SE","stop_name_12":" Ardee Court","stop_id_12":2313,"stop_distance_12":"474m","stop_bearing_12":"SE","stop_name_13":" Cork Street","stop_id_13":4854,"stop_distance_13":"479m","stop_bearing_13":"SE","stop_name_14":" Brabazon Street","stop_id_14":5025,"stop_distance_14":"482m","stop_bearing_14":"E","stop_name_15":" Chamber Street","stop_id_15":309,"stop_distance_15":"511m","stop_bearing_15":"SE","stop_name_16":" Thomas Road","stop_id_16":308,"stop_distance_16":"574m","stop_bearing_16":"SE"}');
//         setTimeout(function(){
//           appMessageQueue.send(message);  
//         }, 1000);
//         return;
//         // END DEBUG
        
        var stops = db2go.storedStops();
        db2goLocation.get(function(){
          db2go.listStops(stops);
        });
        
        break;
      case actions.removeStop:
        var stopIds = JSON.parse(localStorage.getItem("stops")) || [];
        var tempIds = [];
        stopIds.forEach(function(stopId){
          if(message.id != stopId){
            tempIds.push(stopId);    
          }
        });
        
        appMessageQueue.send({
          "action" : actions.removeStop,
          "status" : sendStatus.end
        });
        localStorage.setItem("stops", JSON.stringify(tempIds));
        console.log('Stop Removed: '+ stopIds); 
        break;
        
      case actions.saveStop:
        var stops = window['stops'] || JSON.parse(localStorage.getItem("stopsCache"));
        var stopIds = JSON.parse(localStorage.getItem("stops")) || [];
        stops.forEach(function(stop){
          if(message.id == stop.stopnumber){
            stopIds.push(message.id);
            localStorage.setItem("stops", JSON.stringify(stopIds));
            localStorage.setItem(message.id, JSON.stringify(stop));

            appMessageQueue.send({
              "action" : actions.saveStop,
              "status" : sendStatus.end
            });
            console.log('Stop Saved: '+ stopIds); 
          }
        });
        break;
        
      case actions.getNearestStops:
        console.log("Get Nearest Stops: Start");
        
//         // START DEBUG
//         var message = JSON.parse('{"action":3,"status":2,"stop_name_0":" Watling St","stop_id_0":1997,"stop_distance_0":"295m","stop_bearing_0":"N","stop_name_1":" Watling Street","stop_id_1":1940,"stop_distance_1":"296m","stop_bearing_1":"N","stop_name_2":" Bridgefoot Street","stop_id_2":1939,"stop_distance_2":"344m","stop_bearing_2":"NE","stop_name_3":" Marion Villas","stop_id_3":2379,"stop_distance_3":"347m","stop_bearing_3":"S","stop_name_4":" Bridgefoot Street","stop_id_4":1998,"stop_distance_4":"362m","stop_bearing_4":"NE","stop_name_5":" Ardee St","stop_id_5":2314,"stop_distance_5":"382m","stop_bearing_5":"SE","stop_name_6":" Donore Avenue","stop_id_6":2315,"stop_distance_6":"384m","stop_bearing_6":"S","stop_name_7":" Echlin Street","stop_id_7":1996,"stop_distance_7":"387m","stop_bearing_7":"NW","stop_name_8":" Ardee St","stop_id_8":2380,"stop_distance_8":"400m","stop_bearing_8":"SE","stop_name_9":" Steevens Lane","stop_id_9":1941,"stop_distance_9":"404m","stop_bearing_9":"NW","stop_name_10":" Brabazon Street","stop_id_10":7412,"stop_distance_10":"425m","stop_bearing_10":"E","stop_name_11":" Brabazon Row","stop_id_11":2382,"stop_distance_11":"472m","stop_bearing_11":"SE","stop_name_12":" Ardee Court","stop_id_12":2313,"stop_distance_12":"474m","stop_bearing_12":"SE","stop_name_13":" Cork Street","stop_id_13":4854,"stop_distance_13":"479m","stop_bearing_13":"SE","stop_name_14":" Brabazon Street","stop_id_14":5025,"stop_distance_14":"482m","stop_bearing_14":"E","stop_name_15":" Chamber Street","stop_id_15":309,"stop_distance_15":"511m","stop_bearing_15":"SE","stop_name_16":" Thomas Road","stop_id_16":308,"stop_distance_16":"574m","stop_bearing_16":"SE"}');
//         setTimeout(function(){
//           appMessageQueue.send(message);  
//         }, 1000);
//         return;
//         // END DEBUG
        
        db2goLocation.get(function(){
          db2go.getStops(function(stops){
            window['stops'] = stops;
            db2go.listStops(stops);
            
            setTimeout(function(){
              localStorage.setItem("stopsCache", JSON.stringify(stops));
            },0);
          });  
        });
        break;
        
      case actions.getStop:
        var stopId = message.id;

        console.log("Get Stop: " +stopId );
        
//         // START DEBUG
//         var message = JSON.parse('{"action":2,"status":2,"id":"1997","bus_route_0":"123","bus_destination_0":"Marino","bus_duein_0":2,"bus_route_1":"40","bus_destination_1":"Finglas","bus_duein_1":10,"bus_route_2":"123","bus_destination_2":"Marino","bus_duein_2":12,"bus_route_3":"13","bus_destination_3":"Harristown","bus_duein_3":17,"bus_route_4":"40","bus_destination_4":"Finglas","bus_duein_4":24,"bus_route_5":"123","bus_destination_5":"Marino","bus_duein_5":29,"bus_route_6":"13","bus_destination_6":"Harristown","bus_duein_6":31,"bus_route_7":"40","bus_destination_7":"Finglas","bus_duein_7":46,"bus_route_8":"123","bus_destination_8":"Marino","bus_duein_8":49,"bus_route_9":"13","bus_destination_9":"Harristown","bus_duein_9":56}');
//         setTimeout(function(){
//           appMessageQueue.send(message);  
//         }, 1000);
//         return;
//         // END DEBUG
        
        db2go.getStop(stopId, function(xml, text, a){
          if(!xml){
            db2go.parseStopApple(text, a);  
          }
          else{
            db2go.parseStopAndroid(xml, a);
          }
        });
        break;
    }
  });

Pebble.addEventListener('showConfiguration', function(e) {
  var stops = db2go.storedStops();
  var message = db2go.encodeStops(stops);
  var configPage = 'http://192.168.1.7:8080/';
  
  console.log('Show Configuration');
  Pebble.openURL(configPage+'#'+message);
});

Pebble.addEventListener('webviewclosed',
  function(e) {
    var configuration = JSON.parse(decodeURIComponent(e.response));
    console.log('Configuration window returned: ', JSON.stringify(configuration));
  }
);