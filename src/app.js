
var db2go = db2go || {
  stopDetailsUrl: 'http://rtpi.dublinbus.ie/DublinBusRTPIService.asmx?op=GetRealTimeStopData',
  geoLookupUrl: 'http://dublinbus.ie/Templates/Public/RoutePlannerService/RTPIMapHandler.ashx?&rjson=true',
  
  getXML : function (url, data, callback) {
    var xmlhttp = new XMLHttpRequest();
    var data = data || '';
    var callback = callback || function(){};

    xmlhttp.open('POST', url, true);
    xmlhttp.onreadystatechange = function () {
      if (xmlhttp.readyState == 4) {
        if (xmlhttp.status == 200) {
          var response = xmlhttp.responseXML;
          callback.call(this, response);
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
    var stopI = (stopI * stopI/2)+1 || 1;
  
    var _this = this;
    var minStopSize = 10;
    var latDiff = 0.0015;
    var lngDiff = 0.002;
  
    if(!db2goLocation.coords){
      db2goLocation.storedCallback = arguments.callee;
      db2goLocation.storedArguements = arguments;
      
      return false;
    }
    
    var lat = db2goLocation.coords.latitude;
    var lng =  db2goLocation.coords.longitude;
  
    var neCoords = (lat+(stopI * latDiff)) +','+ (lng+(stopI * lngDiff));
    var swCoords = (lat-(stopI * latDiff)) +','+ (lng-(stopI * lngDiff));
    var requestUrl = db2go.geoLookupUrl + '&ne=' + neCoords + '&sw=' + swCoords;
    
    db2go.getJSON(requestUrl, function (stops){
      if(stops.points.length < minStopSize && stopI < 10){
        console.log('More stop needed:', stops.points.length);
        db2go.getStops(callback, stopI);
      }
      else{
        callback.call(this, stops.points);
      }
    });
  },
  listStops: function(stops){
    if(!db2goLocation.coords){
      db2goLocation.storedCallback = arguments.callee;
      db2goLocation.storedArguements = arguments;
      
      return false;
    }
    
    var here = new LatLon(db2goLocation.coords.latitude, db2goLocation.coords.longitude);
    
    console.log(stops.length + " stop nearby");
    var message = {
      "action": actions.getStops,
      "status" : sendStatus.end
    };

    var serialisedStops = '';
    var maxResults = 15;

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

      stops[index].rawDistance = rawDistance;
      stops[index].distance = distance;
      stops[index].bearing = bearing;
      stops[index].name = stop.address.split(',')[0];
      stops[index].id = parseInt(stops[index].stopnumber)
    });

    stops.sort( function(a, b){ // Sorting
      return a.rawDistance - b.rawDistance;
    });
    stops.splice(maxResults);

    stops.forEach(function(stop, index){
      message["stop_name_"+index]= stop.name;
      message["stop_id_"+index] = stop.id;
      message["stop_distance_"+index] = stop.distance;
      message["stop_bearing_"+index] = stop.bearing;
      message["stop_index_"+index] = index;
    });

    appMessageQueue.send(message);
    console.log("LIST STOPS: End");
  }
};

var db2goLocation = {
  coords: false,
  storedCallback: false,
  storedArguements: false,
  options: {
    enableHighAccuracy: true, 
    maximumAge: 10000, 
    timeout: 10000
  },
  success: function(pos) {
    db2goLocation.coords = pos.coords;
    
    console.log('Watch at: ' + db2goLocation.coords.latitude + ',' + db2goLocation.coords.longitude);

    if(db2goLocation.storedCallback){
      db2goLocation.storedCallback.call(this, db2goLocation.storedArguements[0], db2goLocation.storedArguements[1], db2goLocation.storedArguements[2]); 
      db2goLocation.storedCallback = false;
      db2goLocation.storedArguements = false;
    }
  },
  error: function(err) {
    console.log('location error (' + err.code + '): ' + err.message);
  }  
};

var actions = {
  getSavedStops: 0,
  getNearestStops: 1,
  getStop:  2,
  getStops:  3,
  saveStop:  4,
  removeStop:  5
};

var sendStatus = {
  start: 0,
  inProgress: 1,
  end:  2,
  error: 3
}

Pebble.addEventListener('ready',
 function(e) {
    // Request current position
    console.log('Pebble Ready');
    navigator.geolocation.watchPosition(db2goLocation.success, db2goLocation.error, db2goLocation.options);  
 }
);

Pebble.addEventListener("appmessage",
  function(e) {
    var message = e.payload;
    console.log("Message: " + message.action);
    console.log(JSON.stringify(message));

    switch(message.action){
      case actions.getSavedStops:
        var stops = [];
        console.log("Get Saved Stops: Start");
        
        var stopIds = JSON.parse(localStorage.getItem("stops"));
        stopIds.forEach(function(stopId){
          var stop = JSON.parse(localStorage.getItem(stopId));
          stops.push(stop);
        });
        
        db2go.listStops(stops);
        
        break;
      case actions.removeStop:
        var stopIds = JSON.parse(localStorage.getItem("stops")) || [];
        var tempIds = [];
        stopIds.forEach(function(stopId){
          if(message.id != stopId){
            tempIds.push(stopId);    
          }
        });
        
        localStorage.setItem("stops", JSON.stringify(tempIds));
        appMessageQueue.send({
          "action" : actions.removeStop,
          "status" : sendStatus.end
        });
        console.log('Stop Removed: '+ stopIds); 
        break;
      case actions.saveStop:
        window['stops'].forEach(function(stop){
          var stopIds = JSON.parse(localStorage.getItem("stops")) || [];

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
       
        db2go.getStops(function(stops){
          window['stops'] = stops;
          db2go.listStops(stops);
        });    
        break;
      case actions.getStop:
        var stopId = message.id;

        console.log("Get Stop: " +stopId );
        db2go.getStop(stopId, function(xml, a){
          var maxResponse = 10;
          var response = xml.getElementsByTagName("DocumentElement")[0];
          var message  = {
              "action": actions.getStop,
              "status" : sendStatus.end,
              "id" : ""+stopId+""
            }

          if(!response){
            appMessageQueue.send({
              "action": actions.getStop,
              "status" : sendStatus.error
            });  
            return false;
          }
          
          var responseSize = response.childNodes.length;
          responseSize = responseSize < maxResponse ? responseSize : maxResponse;
          
          for(var i=0; i<responseSize; i++){
            var stop = response.childNodes[i].childNodes;

            var route = stop[9].textContent;
            var destination = stop[16].textContent.split(" via ")[0];
            var now = new Date(stop[27].textContent);
            var expected = new Date(stop[24].textContent);
            var dueIn = Math.round(((expected-now)/60)/1000);
            
            message['bus_route_'+i] = route;
            message['bus_destination_'+i] = destination;
            message['bus_duein_'+i] = dueIn;
            
            console.log(route +' '+ destination + ' - ' + dueIn + ' mins');
          }  
          
          appMessageQueue.send(message);
        });
        break;
    }
  });