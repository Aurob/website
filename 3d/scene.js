import * as THREE from './three.module.js';

import { PointerLockControls } from './PointerLockControls.js';

let camera, scene, renderer, controls;

const objects = [];

let raycaster;

let moveForward = false;
let moveBackward = false;
let moveLeft = false;
let moveRight = false;
let canJump = false;

let prevTime = performance.now();
const velocity = new THREE.Vector3();
const direction = new THREE.Vector3();
const vertex = new THREE.Vector3();
const color = new THREE.Color();

let base_height = 10;
let jump_velocity = 400;
let playerbbox;
let artists;
document.getElementById("pause_text").innerHTML = "Loading...";
// fetch('artists.json')
//     .then(res => res.json())
//     .then(data => {
//         artists = data;
//     }).then(function () {

//         init();
//         animate();
//     });

init();
animate();

function init() {

    camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 1, 1000);
    camera.position.y = base_height;

    scene = new THREE.Scene();
    scene.background = new THREE.Color(0xffffff);
    scene.fog = new THREE.Fog(0xffffff, 0, 750);

    const light = new THREE.HemisphereLight(0xeeeeff, 0x777788, 0.75);
    light.position.set(0.5, 1, 0.75);
    scene.add(light);

    controls = new PointerLockControls(camera, document.body);

    const blocker = document.getElementById('blocker');
    const instructions = document.getElementById('instructions');

    instructions.addEventListener('click', function () {

        controls.lock();

    });

    controls.addEventListener('lock', function () {

        instructions.style.display = 'none';
        blocker.style.display = 'none';

    });

    controls.addEventListener('unlock', function () {

        blocker.style.display = 'block';
        instructions.style.display = '';

    });

    scene.add(controls.getObject());

    const onKeyDown = function (event) {
        switch (event.code) {

            case 'ArrowUp':
            case 'KeyW':
                moveForward = true;
                break;

            case 'ArrowLeft':
            case 'KeyA':
                moveLeft = true;
                break;

            case 'ArrowDown':
            case 'KeyS':
                moveBackward = true;
                break;

            case 'ArrowRight':
            case 'KeyD':
                moveRight = true;
                break;

            case 'Space':
                if (canJump === true) velocity.y += jump_velocity;
                canJump = false;
                break;

            case 'ControlRight':
                addArtObject(1, controls.getObject().position.x - 20, controls.getObject().position.z - 20, 1, true);
                break;

            case 'KeyP':
                console.log(camera.rotation);
                break;
        }

    };

    const onKeyUp = function (event) {

        switch (event.code) {

            case 'ArrowUp':
            case 'KeyW':
                moveForward = false;
                break;

            case 'ArrowLeft':
            case 'KeyA':
                moveLeft = false;
                break;

            case 'ArrowDown':
            case 'KeyS':
                moveBackward = false;
                break;

            case 'ArrowRight':
            case 'KeyD':
                moveRight = false;
                break;

        }

    };

    document.addEventListener('keydown', onKeyDown);
    document.addEventListener('keyup', onKeyUp);

    raycaster = new THREE.Raycaster(new THREE.Vector3(), new THREE.Vector3(0, - 1, 0), 0, 10);

    // floor

    let floorGeometry = new THREE.PlaneGeometry(2000, 2000, 100, 100);
    floorGeometry.rotateX(- Math.PI / 2);

    // vertex displacement

    let position = floorGeometry.attributes.position;
    floorGeometry = floorGeometry.toNonIndexed(); // ensure each face has unique vertices

    position = floorGeometry.attributes.position;
    const colorsFloor = [];

    for (let i = 0, l = position.count; i < l; i++) {
        color.setHSL(0.5, 0.75, 0.75);
        colorsFloor.push(color.r, color.g, color.b);

    }

    floorGeometry.setAttribute('color', new THREE.Float32BufferAttribute(colorsFloor, 3));

    const floorMaterial = new THREE.MeshBasicMaterial({ vertexColors: true });

    const floor = new THREE.Mesh(floorGeometry, floorMaterial);
    scene.add(floor);

    //Create a wall of paintings
    createWallOfArt(10, 10, 10, 1, 2);
    createWallOfArt(10, 10, 40, 1, 2);
    const playerGeometry = new THREE.BoxGeometry(3, base_height * 2, 3).toNonIndexed();

    playerbbox = new THREE.Mesh(playerGeometry);

    renderer = new THREE.WebGLRenderer({ antialias: true });
    renderer.setPixelRatio(window.devicePixelRatio);
    renderer.setSize(window.innerWidth, window.innerHeight);
    document.body.appendChild(renderer.domElement);
    window.addEventListener('resize', onWindowResize);
    document.getElementById("pause_text").innerHTML = "Click to play";

}

function createWallOfArt(count, x, z, direction = 1, offset = 5) {
    //TEMP REMOVE
    // objects.forEach(object=> {
    //     scene.remove(object);
    // });
    let width, depth;
    var max_width = 100;
    for(let c = 0; c < 10; c++) {
        if(max_width < 0) break;
        let e = fetch(`get_art.php?count=1`).then(res => res.json()).then(function (data) {
            
            let image = data[0];
            let floorOffset = 5;
            var frame_group = new THREE.Group();

            let texture = new THREE.TextureLoader().load(image['image_b64']);
            const picture_material = new THREE.MeshBasicMaterial({ map: texture });
            
            let lastw = width, lastd = depth;
            width = (direction == 1) ? image['size'][0] / 100 : .2;
            depth = (direction == 1) ? .2 : image['size'][0] / 100;
            
            if(c > 0) {
                if(direction == 1)
                    x += lastw/2 + width/2 + offset;
                else
                    z += lastd/2 + depth/2 + offset;
            }
            const picture_geometry = new THREE.BoxGeometry(width, image['size'][1] / 100, depth);
            const picture_mesh = new THREE.Mesh();
            picture_mesh.geometry = picture_geometry;
            picture_mesh.material = picture_material;

            picture_mesh.position.x = x;
            picture_mesh.position.y = picture_geometry.parameters.height / 2 + floorOffset;
            picture_mesh.position.z = z;

            //TODO
            //USE ONLY 1 HORIZONAL AND 1 VERTICAL FRAME MESH
            // const horizontal_frame_geo_top = new THREE.BoxGeometry( picture_geometry.parameters.width/.95, picture_geometry.parameters.height/30, 1);
            // const horizontal_frame_mat_top = new THREE.MeshBasicMaterial({ color: 0xc4970a});
            // const horizontal_frame_mesh_top = new THREE.Mesh( horizontal_frame_geo_top, horizontal_frame_mat_top );
            // horizontal_frame_mesh_top.position.x = picture_mesh.position.x;
            // horizontal_frame_mesh_top.position.y = picture_mesh.position.y*2 - floorOffset + horizontal_frame_geo_top.parameters.height/2;
            // horizontal_frame_mesh_top.position.z = picture_mesh.position.z;

            // const horizontal_frame_geo_btm = new THREE.BoxGeometry( picture_geometry.parameters.width/.95, picture_geometry.parameters.height/30, 1);
            // const horizontal_frame_mat_btm = new THREE.MeshBasicMaterial({ color: 0xc4970a});
            // const horizontal_frame_mesh_btm = new THREE.Mesh( horizontal_frame_geo_btm, horizontal_frame_mat_btm );
            // horizontal_frame_mesh_btm.position.x = picture_mesh.position.x;
            // horizontal_frame_mesh_btm.position.y = floorOffset - horizontal_frame_geo_btm.parameters.height/2;
            // horizontal_frame_mesh_btm.position.z = picture_mesh.position.z;


            // const vertical_frame_geo_left = new THREE.BoxGeometry( horizontal_frame_geo_top.parameters.height, picture_geometry.parameters.height/.95, 1);
            // const vertical_frame_mat_left = new THREE.MeshBasicMaterial({ color: 0xc4970a});
            // const vertical_frame_mesh_left = new THREE.Mesh( vertical_frame_geo_left, vertical_frame_mat_left );
            // vertical_frame_mesh_left.position.x = picture_mesh.position.x - picture_geometry.parameters.width/2 - vertical_frame_geo_left.parameters.width/2;
            // vertical_frame_mesh_left.position.y = picture_mesh.position.y;
            // vertical_frame_mesh_left.position.z = picture_mesh.position.z;

            // const vertical_frame_geo_right = new THREE.BoxGeometry( horizontal_frame_geo_top.parameters.height, picture_geometry.parameters.height/.95, 1);
            // const vertical_frame_mat_right = new THREE.MeshBasicMaterial({ color: 0xc4970a});
            // const vertical_frame_mesh_right = new THREE.Mesh( vertical_frame_geo_right, vertical_frame_mat_right );
            // vertical_frame_mesh_right.position.x = picture_mesh.position.x + picture_geometry.parameters.width/2 + vertical_frame_geo_right.parameters.width/2;
            // vertical_frame_mesh_right.position.y = picture_mesh.position.y;
            // vertical_frame_mesh_right.position.z = picture_mesh.position.z;
            // // frame_group.add(picture_mesh);
            // frame_group.add(horizontal_frame_mesh);

            //TODO
            //DYNAMIC DIRECTIONS
            // if(playerMade) {

            //     [ vertical_frame_mesh_left, vertical_frame_mesh_right].forEach(
            //         mesh => {
            //             mesh.rotation.y = camera.rotation.y;
            //         }
            //     )
            // }

            scene.add(picture_mesh);
            objects.push(picture_mesh);

            // scene.add(horizontal_frame_mesh_top);
            // objects.push(horizontal_frame_mesh_top);

            // scene.add(horizontal_frame_mesh_btm);
            // objects.push(horizontal_frame_mesh_btm);

            // scene.add(vertical_frame_mesh_left);
            // objects.push(vertical_frame_mesh_left);

            // scene.add(vertical_frame_mesh_right);
            // objects.push(vertical_frame_mesh_right);

            // if(direction == 1)
            //     wall_width -= x;
            // else
            //     wall_width -= z;
            return max_width-x;
        });
        console.log(e, max_width);
    }
}
function onWindowResize() {

    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();

    renderer.setSize(window.innerWidth, window.innerHeight);

}

function animate() {

    requestAnimationFrame(animate);

    const time = performance.now();

    if (controls.isLocked === true) {

        raycaster.ray.origin.copy(controls.getObject().position);
        raycaster.ray.origin.y -= base_height;
        const intersections = raycaster.intersectObjects(objects);

        const onObject = intersections.length > 0;
        // const colliding = (intersections.length > 0 && intersections[0].distance < cameraDirection.length());

        const delta = (time - prevTime) / 1000;
        const last_position = controls.getObject().position;

        velocity.x -= velocity.x * 10.0 * delta;
        velocity.z -= velocity.z * 10.0 * delta;

        velocity.y -= 9.8 * 100.0 * delta; // 100.0 = mass

        direction.z = Number(moveForward) - Number(moveBackward);
        direction.x = Number(moveRight) - Number(moveLeft);
        direction.normalize(); // this ensures consistent movements in all directions

        if (moveForward || moveBackward) velocity.z -= direction.z * 400.0 * delta;
        if (moveLeft || moveRight) velocity.x -= direction.x * 400.0 * delta;

        if (onObject === true) {
            console.log(true);
            velocity.y = Math.max(0, velocity.y);
            canJump = true;

        }

        let collision = false;
        for (let o = 0; o < objects.length; o++) {

            if (detectCollisionCubes(playerbbox, objects[o])) {
                collision = true;
                break;
            }
        }

        if (collision) {
            // controls.moveRight(velocity.x * delta*2);
            // controls.moveForward(velocity.z * delta*2);
            console.log(true);
            velocity.x = -velocity.x; //Math.max(0, velocity.x);
            velocity.z = -velocity.z; //Math.max(0, velocity.z);
        }

        controls.moveRight(- velocity.x * delta);
        controls.moveForward(- velocity.z * delta);
        controls.getObject().position.y += (velocity.y * delta); // new behavior

        if (controls.getObject().position.y < base_height) {

            velocity.y = 0;
            controls.getObject().position.y = base_height;

            canJump = true;

        }



        playerbbox.position.x = controls.getObject().position.x;
        playerbbox.position.y = controls.getObject().position.y;
        playerbbox.position.z = controls.getObject().position.z;

    }

    prevTime = time;

    renderer.render(scene, camera);

}

function detectCollisionCubes(object1, object2) {
    object1.geometry.computeBoundingBox(); //not needed if its already calculated
    object2.geometry.computeBoundingBox();
    object1.updateMatrixWorld();
    object2.updateMatrixWorld();

    var box1 = object1.geometry.boundingBox.clone();
    box1.applyMatrix4(object1.matrixWorld);

    var box2 = object2.geometry.boundingBox.clone();
    box2.applyMatrix4(object2.matrixWorld);

    return box1.intersectsBox(box2);
}
