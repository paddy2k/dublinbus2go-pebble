var Utils = {
  prettyBearing : function(rawBearing){
    // Offset bearing by 45 deg
    var offset = 22.5;
    var correctedBearing = (rawBearing + 22.5)%360;
    
    if(correctedBearing <= 45){
      bearing = 'N';
    }
    else if(correctedBearing <= 90){
      bearing = 'NE';
    }
    else if(correctedBearing <= 135){
      bearing = 'E';
    }
    else if(correctedBearing <= 180){
      bearing = 'SE';
    }
    else if(correctedBearing <= 225){
      bearing = 'S';
    }
    else if(correctedBearing <= 270){
      bearing = 'SW';
    }
    else if(correctedBearing <= 315){
      bearing = 'W';
    }
    else if(correctedBearing <= 360){
      bearing = 'NW';
    }
    
    return bearing;
  },
  
  testPrettyBearing: function (deg, bearing){
    console.log('Prettifing '+deg+'°, expects '+bearing);
    
    var prettyBaring = Utils.prettyBearing(deg);
    var result = prettyBaring==bearing ? 'PASS' : 'FAIL';
    console.log('Result: '+ result);
  },
  
  prettyDistance : function(rawDistance){
    var distance;
    
    if(rawDistance<1){
      distance = Math.floor(rawDistance*1000) + 'm';
    }
    else {
      distance = (Math.round(rawDistance * 100) / 100) + 'km';
    }

    return distance;
  }
}