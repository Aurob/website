<?php
    
    ini_set('display_errors', 1);
    ini_set('display_startup_errors', 1);
    error_reporting(E_ALL);

    function new_file($name, $content, $key=false){
        $name = preg_replace("/[^a-zA-Z0-9]/", '', $name);
        if(!empty($name)) {
            $path = "documents/" . $name . ".json";
            if ( !file_exists("documents") ) {
                mkdir ("documents", 0744);
            }
        
            if(!file_exists($path)) { 
                $note = ["content" => $content];
                if($key) {
                    $note['key'] = $key;
                }

                file_put_contents($path, json_encode($note));
                return true;
            }
        }
        return false;
    }

    function load_file($name, $key=false){
        $name = preg_replace("/[^a-zA-Z0-9]/", '', $name);
        $path = "documents/" . $name . ".json";
        if(file_exists($path)) {
            $note_json = json_decode(file_get_contents($path), true);
            $note = ["content" => $note_json['content']];
            if(array_key_exists("key", $note_json)) {
                if($note_json["key"] != $key) return false;
                else $note["key"] = true;
            }
            
            return $note;
        }
        else return false;
        
    }

    header('Content-Type: application/json');
    $response = array();
    if(isset($_REQUEST['o'])) {
        $key = false;
        if(isset($_REQUEST['k'])) $key = $_REQUEST['k'];
        switch($_REQUEST['o']) {
            case 'new':
                if(isset($_REQUEST['f'], $_REQUEST['c'])) {
                    $add_file = new_file($_REQUEST['f'], $_REQUEST['c'], $key);
                    $response["result"] = $add_file;
                }
                break;
            
            case 'load':
                if(isset($_REQUEST['f'])) {
                    $content = load_file($_REQUEST['f'], $key);
                    $response["result"] = $content;
                }
                break;
        }

    }

    echo json_encode($response);
?>
