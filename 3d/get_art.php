<?php
// header('Content-Type: application/json');

if(isset($_GET['test'])) {
 
    ini_set('display_errors', 1);
    ini_set('display_startup_errors', 1);
    error_reporting(E_ALL);
    $rand_row = rand(0, 100);
    
    $db = new SQLite3('/var/www/html/art/artvee.db');
    $res = $db->query('tables');
    $return = [];
    while ($row = $res->fetchArray()) {
        // $url = $row['link'];
        // $img = file_get_contents($url);
        // $b64image = "data:image/jpeg;base64," . base64_encode($img);
        // $return['size'] = getimagesize($b64image);
        $return = $row;
    }

    echo json_encode($return);
}
$response = array();
$art_count = 0;

if(isset($_GET['count'])){   
    $art_count = $_GET['count']; 
}

$art_list = [];
if($art_count > 0) {
    
    $db = new SQLite3('/var/www/html/art/artvee.db');
    $count = $db->querySingle("SELECT COUNT(*) as count FROM art");

    $art_ids = [];
    for($i = 0; $i < $art_count; $i++) {
        //Get a random art ID and check if the ID has already been used
        // if it has, decrement $i and generate another ID
        $rand_row = rand(0, $count-1);
        if(in_array($rand_row, $art_ids)) {
            $i--;
        }
        else {
            //Get the artwork link, convert it to a base64 string and store it along with it's dimensions
            $res = $db->query('SELECT * FROM art WHERE rowid = '.$rand_row);
            while ($row = $res->fetchArray()) {
                $url = $row['link'];
                $img = file_get_contents($url);
                $b64image = "data:image/jpeg;base64," . base64_encode($img);
                $art_data['image_b64'] = $b64image;
                $art_data['size'] = getimagesize($b64image);
            }
            //Push the art ID to the art id list so it won't be duplicated
            array_push($art_ids, $rand_row);
            //finally push the base64 string and dimension data
            array_push($art_list, $art_data);
        }
    }
}

echo json_encode($art_list);
?>