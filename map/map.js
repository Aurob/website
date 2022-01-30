var map;
var trail_markers = [];
var poi_markers = [];
$(document).ready(function() {

    $('#t-btn').on('click', () => {
        showTrails();
    });

    $('#q-btn').on('click', () => {
        queryLocation();
    });

    $('#p-btn').on('click', () => {
        showPOI(50);
    });

    $('#query').on('keyup', function (e) {
        if (e.key === 'Enter' || e.keyCode === 13) {
            $('#q-btn').click();
        }
    });
        
    init();
});

function init() {
    map = L.map('map').setView([32.491230287947594, -93.72264862060548], 10);

    var tile_layer = L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
        attribution: 'Map data &copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors, Imagery © <a href="https://www.mapbox.com/">Mapbox</a>',
    }).addTo(map);

    tile_layer.on('load', function () {
        map.invalidateSize();
        tile_layer.off('load');
    });
}

function queryLocation() {
    $('#q-btn').prop('disabled', true);
    $('#query').blur();
    let q = $('#query').val();
    fetch(`https://nominatim.openstreetmap.org/search?q=${q}&format=json`)
        .then(res => res.json())
        .then(data => {
            if (data.length > 0) {
                map.setView([data[0].lat, data[0].lon], map.getZoom());
            }
            else alert('no results');

            $('#q-btn').prop('disabled', false);
        });
}

function showTrails() {
    let center = map.getCenter();
    let lat = center.lat;
    let lon = center.lng;
    fetch(`https://nominatim.openstreetmap.org/reverse?lat=${lat}&lon=${lon}&format=json`)
        .then(res => res.json())
        .then(data => {
            if(data.address.country_code == 'us') {
                let state = data.address.state;
                fetch(`index.php?state=${state}`)
                    .then(res => res.json())
                    .then(data => {
                        let bbox = map.getBounds();
                        let min_lat = bbox._southWest.lat;
                        let min_lon = bbox._southWest.lng;
                        let max_lat = bbox._northEast.lat;
                        let max_lon = bbox._northEast.lng;
                        let id = data.id;
                        fetch(`index.php?id=${id}&min_lat=${min_lat}&min_lon=${min_lon}&max_lat=${max_lat}&max_lon=${max_lon}`)
                            .then(res => res.json())
                            .then(data => {
                                if(data.length > 0) {
                                    for(let t = 0; t < trail_markers.length; t++) {
                                        trail_markers[t].remove();
                                    }
                                    trail_markers = [];

                                    for(let t = 0; t < data.length; t++) {
                                        let trail = data[t];
                                        trail_markers.push(L.marker([trail.latitude, trail.longitude]).addTo(map));
                                    }
                                }
                            });
                    })
            }
        });
}

function showPOI(kilometers) {
    let radius = kilometers * 1000;
    let center = map.getCenter();
    let lon = center.lng;
    let lat = center.lat;
    fetch(`https://api.opentripmap.com/0.1/en/places/radius?kinds=museums&radius=${radius}&lon=${lon}&lat=${lat}&apikey=5ae2e3f221c38a28845f05b6934819604949c558a3d71a27e7647681`)
    .then(res => res.json())
    .then(data => {
        if(data.features.length > 0) {
            for(let p = 0; p < poi_markers.length; p++) {
                poi_markers[p].remove();
            }
            poi_markers = [];
            
            for(let p = 0; p < data.features.length; p++) {
                let poi = data.features[p].geometry.coordinates;
                let marker = L.marker([poi[1], poi[0]]).addTo(map);
                marker._icon.style.filter = "hue-rotate(-90deg)";
                poi_markers.push(marker);
            }
        }

    });
}