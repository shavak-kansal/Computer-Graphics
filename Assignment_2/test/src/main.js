

import { Water } from 'https://threejs.org/examples/jsm/objects/Water.js';

import { Sky } from 'https://threejs.org/examples/jsm/objects/Sky.js';


const scene = new THREE.Scene();

function buildSky() {
    const sky = new Sky();
    sky.scale.setScalar(10000);
    scene.add(sky);
    return sky;
}

function Car(){
    const car = new THREE.Group();

    const bWHeel = new THREE.Mesh(
        new THREE.BoxBufferGeometry(40, 12, 12),    // width, height, depth x,y,z
        new THREE.MeshBasicMaterial({color: 0xffffff})
    );

    bWHeel.position.set(0, 8, -8);

    const fWheel = new THREE.Mesh(
        new THREE.BoxBufferGeometry(40, 12, 12),
        new THREE.MeshBasicMaterial({color: 0xffffff})
    );

    fWheel.position.set(0, 8, 8);

    car.add(bWHeel);
    car.add(fWheel);
    
    const Cabin = new THREE.Mesh(
        new THREE.BoxBufferGeometry(32, 16, 30),
        new THREE.MeshBasicMaterial({color: 0xf00000})
    );

    Cabin.position.y = 16

    car.add(Cabin);

    return car;
}


const playerCar = Car();
scene.add(playerCar);

const ambientLight = new THREE.AmbientLight(0xfffff0, 0.5);
scene.add(ambientLight);

const dirLight = new THREE.DirectionalLight(0xffffff, 0.5);
dirLight.position.set(0, 100, -100);
scene.add(dirLight);

const camera = new THREE.PerspectiveCamera( 75, window.innerWidth / window.innerHeight, 0.1, 1000 );

//camera.position.x = 1;
camera.position.z = 3*Math.cos(Math.PI/6);
camera.position.y = 3*Math.sin(Math.PI/6);

camera.position.set(150, 50, 150);

//buildSky();

camera.lookAt(playerCar.position);


const renderer = new THREE.WebGLRenderer({antialias: true});
renderer.setSize(window.innerWidth, window.innerHeight);
document.body.appendChild(renderer.domElement);



renderer.render(scene, camera);

function animate() {
	requestAnimationFrame( animate );
	renderer.render( scene, camera );
}
animate();