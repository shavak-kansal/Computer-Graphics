import "./style.css";

import * as THREE from "three";
import { OrbitControls } from "three/examples/jsm/controls/OrbitControls.js";
import { Water } from "three/examples/jsm/objects/Water.js";
import { Sky } from "three/examples/jsm/objects/Sky.js";
import { GLTFLoader } from "three/examples/jsm/loaders/GLTFLoader.js";
import { GUI } from "three/examples/jsm/libs/lil-gui.module.min.js";
import Stats from 'three/examples/jsm/libs/stats.module'
import { Camera } from "three";

const numOfEnemies = 7;

const scene = new THREE.Scene();
const stats = new Stats();

let up, down, left, right, cameraOverhead;

const playerCar = Car();
//scene.add(playerCar);

var shipModel;
var enemyShipModelRef;

var enemyShipModels = [];
var canonBalls = [];

var camera;
var shipPos = new THREE.Vector3(0, 0, 0);
var directionVec = new THREE.Vector3(0, -50, 100);

setInterval(function(){
  enemyShipModels.forEach(function(enemyShipModel){
    shootCanonBall(enemyShipModel, shipModel.scene, 2000);
  });

  //shootCanonBall(shipModel.scene, enemyShipModels[0]);
}, 4000);

function loadPlayerShip() {
  const loader = new GLTFLoader();

  loader.load(
    "./assets/ship1/scene.gltf",
    function (gltf) {
      
      shipModel = gltf;
      
      enemyShipModelRef = gltf.scene.clone();

      enemyShipModels.forEach(function (enemyShipModel) {
        enemyShipModel.position.set(Math.random() * 1000 - 50,-1,Math.random() * 1000 - 50  );  
        enemyShipModel.scale.set(0.09, 0.09, 0.09);
        scene.add(enemyShipModel);
      });
      shipModel.scene.scale.set(0.1, 0.1, 0.1);
      shipModel.scene.position.set(0, -1, 0);

      scene.add(shipModel.scene);
      document.addEventListener("keydown", onDocumentKeyDown, false);
      document.addEventListener("keyup", onDocumentKeyUp, false);

      function onDocumentKeyDown(event){

        if(event.keyCode == 87){
          up = true;
          //shootCanonBall(shipModel.scene, enemyShipModels[0]);
        }
        if(event.keyCode == 83){
          down = true;
        }
        if(event.keyCode == 65){
          right = true;
        }
        if(event.keyCode == 68){
          left = true;
        }

        if(event.keyCode == 70){
          shootCanonBall(shipModel.scene, enemyShipModels[0]);
        }
        if(event.keyCode == 86){
          cameraOverhead = !cameraOverhead;
        }
      }

      function onDocumentKeyUp(event) {
        if (event.keyCode == 87) {
          up = false;
        }
        if (event.keyCode == 83) {
          down = false;
        }
        if (event.keyCode == 65) {
          right = false;
        }
        if (event.keyCode == 68) {
          left = false;
        }
      }
    },
    undefined,
    function (error) {
      console.error(error);
    }
  );
}

function shootCanonBall(one, two, life = 400) {
  var canonBall = new THREE.Mesh(
    new THREE.SphereGeometry(5, 32, 16),
    new THREE.MeshBasicMaterial({ color: 0xffffff })
  );


  canonBall.position.set(one.position.x + 10, one.position.y + 10, one.position.z+ 10);
  
  var velocityBall = new THREE.Vector3(two.position.x-one.position.x, two.position.y-one.position.y, two.position.z-one.position.z);

  velocityBall = velocityBall.normalize();
  
  canonBall.userData = {
    distanceLeft: life,
    velocity: new THREE.Vector3(velocityBall.x, velocityBall.y, velocityBall.z)
  };

  console.log("velocity: ");
  console.log(canonBall.userData.velocity);
  scene.add(canonBall);

  canonBalls.push(canonBall);
}

function genEnemies(){
  while(enemyShipModels.length < numOfEnemies){
    var cloned = enemyShipModelRef.clone();

    cloned.position.set( Math.random() * 1000 + 50, -1, Math.random() * 1000 + 50 );
    cloned.scale.set(0.09, 0.09, 0.09);
    enemyShipModels.push(cloned);
    scene.add(cloned);
  }
}

function resetScene(canvas) {

  canvas.appendChild(stats.dom);
  const renderer = buildRenderer(canvas);
  createCamera();

  

  const sky = buildSky();
  const water = buildWater();

  const mainLight = new THREE.DirectionalLight("white", 1);
  mainLight.position.set(1, 1, 0);
  scene.add(mainLight);

  function createCamera(){
    camera = new THREE.PerspectiveCamera(90, window.innerWidth / window.innerHeight, 1, 20000);
    camera.position.set(0, 100, -100);
    camera.lookAt(0, 1, 100);

    //camera.lookAt(shipModel.scene.position);
    // const controls = new OrbitControls(camera, renderer.domElement);
    // controls.maxPolarAngle = Math.PI * 0.495;
    // controls.target.set(0, 0, 0);
    // controls.minDistance = 40.0;
    // controls.maxDistance = 200.0;
    // controls.update();
  }

  function buildRenderer(canvas) {
    const renderer = new THREE.WebGLRenderer();
    renderer.setSize(window.innerWidth, window.innerHeight);
    canvas.appendChild(renderer.domElement);
    return renderer;
  }

  function buildSky() {
    const sky = new Sky();
    sky.scale.setScalar(450000);
    scene.add(sky);
    
    const sun = new THREE.Vector3();
    sun.x = 1;
    sun.y = 0;
    sun.z = 1;

    sky.material.uniforms["sunPosition"].value.copy(sun);

    return sky;
  }

  function buildWater() {
    const waterGeometry = new THREE.PlaneGeometry(40000, 40000);
    const water = new Water(waterGeometry, {
      textureWidth: 512,
      textureHeight: 512,
      waterNormals: new THREE.TextureLoader().load(
        "https://raw.githubusercontent.com/mrdoob/three.js/master/examples/textures/waternormals.jpg",
        function (texture) {
          texture.wrapS = texture.wrapT = THREE.RepeatWrapping;
        }
      ),
      alpha: 1.0,
      sunDirection: new THREE.Vector3(),
      sunColor: 0xffffff,
      waterColor: 0x001e0f,
      distortionScale: 3.7,
      fog: scene.fog !== undefined,
    });
    water.rotation.x = -Math.PI / 2;
    scene.add(water);
    return water;
  }

  function playerMove(){

    const playerShipSpeed = 0.2;

    if(up){
      shipModel.scene.position.z += playerShipSpeed;
    }
    if(down){
      shipModel.scene.position.z -= playerShipSpeed;
    }
    if(right){
      shipModel.scene.position.x += playerShipSpeed;
    }
    if(left){
      shipModel.scene.position.x -= playerShipSpeed;
    }
    
    shipPos = shipModel.scene.position;
  }

  function enemyMove(){
    enemyShipModels.forEach(function (enemyShipModel) {
      const pain = new THREE.Vector3(shipPos.x - enemyShipModel.position.x, 0, shipPos.z - enemyShipModel.position.z);
      //enemyShipModel.position += pain.multiplyScalar(0.1);
      
      enemyShipModel.position.x += pain.x*0.005;
      enemyShipModel.position.z += pain.z*0.005;
    });
  }

  function canonBallMove(){
    const canonBallSpeed = 1;
    canonBalls.forEach(function (canonBall, index, array) { 

      //console.log("CanonBall position: ");
      canonBall.position.x += canonBall.userData.velocity.x*canonBallSpeed;
      canonBall.position.y += canonBall.userData.velocity.y*canonBallSpeed;
      canonBall.position.z += canonBall.userData.velocity.z*canonBallSpeed;

      canonBall.userData.distanceLeft -= canonBallSpeed;
      
      if(canonBall.userData.distanceLeft <= 0){
        scene.remove(canonBall);

      }
    });
  }


  this.update = function () {
    water.material.uniforms["time"].value += 1.0 / 60.0;
    playerMove();
    //enemyMove();  
    canonBallMove();
    genEnemies();


    if(cameraOverhead){
      camera.position.set(shipPos.x, shipPos.y + 400, shipPos.z);
      camera.up.set(0, 0, 1);
      camera.lookAt(shipPos);
    }
    else {
      camera.position.set(shipPos.x - directionVec.x, shipPos.y - directionVec.y, shipPos.z - directionVec.z);
      camera.up.set(0, 1, 0);
      camera.lookAt(shipPos);
    }
    
    renderer.render(scene, camera);
    stats.update();
    //camera.lookAt(shipModel.scene.position);
    
  };

  function onWindowResize() {
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(window.innerWidth, window.innerHeight);
  }

  window.addEventListener("resize", onWindowResize);
  
}

function Car() {
  const car = new THREE.Group();

  const bWHeel = new THREE.Mesh(
    new THREE.BoxBufferGeometry(40, 12, 12), // width, height, depth x,y,z
    new THREE.MeshBasicMaterial({ color: 0xffffff })
  );

  bWHeel.position.set(0, 4, -8);

  const fWheel = new THREE.Mesh(
    new THREE.BoxBufferGeometry(40, 12, 12),
    new THREE.MeshBasicMaterial({ color: 0xffffff })
  );

  fWheel.position.set(0, 4, 8);

  car.add(bWHeel);
  car.add(fWheel);

  const Cabin = new THREE.Mesh(
    new THREE.BoxBufferGeometry(32, 16, 30),
    new THREE.MeshBasicMaterial({ color: 0xf00000 })
  );

  Cabin.position.y = 12;

  car.add(Cabin);

  return car;
}

const canvas = document.getElementById("canvas");

loadPlayerShip();
const plswork = new resetScene(canvas);

function animate() {
  requestAnimationFrame(animate);
  plswork.update();
}

animate();
