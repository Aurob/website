<?php
header('Content-Type: application/json');
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

$return = [];
if(isset($_GET['state'])) {
    $state = $_GET['state'];
    $db = new SQLite3('/var/www/website/map/trails.db');
    $res = $db->query('select id from state where name="'.str_replace(' ', '', strtolower($state).'"'));
    $return = [];
    while ($row = $res->fetchArray()) {
        $return = ['id' => $row['id']];
    }
}
else if(isset($_GET['min_lat'])  && isset($_GET['min_lon'])  && isset($_GET['max_lat'])  && isset($_GET['max_lon'])) {
    $min_lat = $_GET['min_lat'];
    $min_lon = $_GET['min_lon'];
    $max_lat = $_GET['max_lat'];
    $max_lon = $_GET['max_lon'];

    $db = new SQLite3('/var/www/website/map/trails.db');
     
    $res = $db->query('SELECT * FROM trail WHERE
                        latitude > '.$min_lat.' AND latitude < '.$max_lat.' AND
                        longitude > '.$min_lon.' AND longitude < '.$max_lon);
    $return = [];
    while ($row = $res->fetchArray()) {
        array_push($return, $row);
    }
}
echo json_encode($return);
?>