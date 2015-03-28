/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
/*  Latitude/longitude spherical geodesy formulae & scripts (c) Chris Veness 2002-2010            */
function LatLon(lat, lon, rad) {
  window['LatLonCache'] = window['LatLonCache'] || {};
  rad = rad || 6371;
      
  this._lat = lat;
  this._lon = lon;
  this._radius = rad;

}

LatLon.prototype.setCache = function(key, value){
  window['LatLonCache'][key] = value;
}

LatLon.prototype.getCache = function(key){
  return window['LatLonCache'][key];
}
LatLon.prototype.distanceTo = function (point, precision) {
  var type = "d";
  var sourceKey = this._lat * (this._lon*100);
  var targetKey = point._lat * (point._lon*100);
  var key = type + (sourceKey<targetKey ? sourceKey+targetKey : targetKey+sourceKey );

  if(!this.getCache(key)){
    if (typeof precision == "undefined") {
        precision = 7
    }
    var R = this._radius;
    var lat1 = this._lat.toRad(),
        lon1 = this._lon.toRad();
    var lat2 = point._lat.toRad(),
        lon2 = point._lon.toRad();
    var dLat = lat2 - lat1;
    var dLon = lon2 - lon1;
    var a = Math.sin(dLat / 2) * Math.sin(dLat / 2) + Math.cos(lat1) * Math.cos(lat2) * Math.sin(dLon / 2) * Math.sin(dLon / 2);
    var c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
    var d = R * c;

    this.setCache(key, d.toPrecisionFixed(precision));
  }
  return this.getCache(key);
};
LatLon.prototype.bearingTo = function (point) {
  var type = "b";
  var sourceKey = this._lat * (this._lon*100);
  var targetKey = point._lat * (point._lon*100);
  var key = type + (sourceKey<targetKey ? sourceKey+targetKey : targetKey+sourceKey );

  if(!this.getCache(key)){
    var lat1 = this._lat.toRad(),
        lat2 = point._lat.toRad();
    var dLon = (point._lon - this._lon).toRad();
    var y = Math.sin(dLon) * Math.cos(lat2);
    var x = Math.cos(lat1) * Math.sin(lat2) - Math.sin(lat1) * Math.cos(lat2) * Math.cos(dLon);
    var brng = Math.atan2(y, x);
    this.setCache(key, (brng.toDeg() + 360) % 360);
  }
  return this.getCache(key);
};
if (typeof (String.prototype.toRad) === "undefined") {
    Number.prototype.toRad = function () {
        return this * Math.PI / 180
    }
}
if (typeof (String.prototype.toDeg) === "undefined") {
    Number.prototype.toDeg = function () {
        return this * 180 / Math.PI
    }
}
if (typeof (Number.prototype.toPrecisionFixed) === "undefined") {
    Number.prototype.toPrecisionFixed = function (precision) {
        var numb = this < 0 ? -this : this;
        var sign = this < 0 ? "-" : "";
        if (numb == 0) {
            n = "0.";
            while (precision--) {
                n += "0"
            }
            return n
        }
        var scale = Math.ceil(Math.log(numb) * Math.LOG10E);
        var n = String(Math.round(numb * Math.pow(10, precision - scale)));
        if (scale > 0) {
            l = scale - n.length;
            while (l-- > 0) {
                n = n + "0"
            }
            if (scale < n.length) {
                n = n.slice(0, scale) + "." + n.slice(scale)
            }
        } else {
            while (scale++ < 0) {
                n = "0" + n
            }
            n = "0." + n
        }
        return sign + n
    }
};