import "./style.css";

import * as THREE from "three";
import { OrbitControls } from "three/examples/jsm/controls/OrbitControls.js";
import { Water } from "three/examples/jsm/objects/Water.js";
import { Sky } from "three/examples/jsm/objects/Sky.js";
import { GLTFLoader } from "three/examples/jsm/loaders/GLTFLoader.js";
import { GUI } from "three/examples/jsm/libs/lil-gui.module.min.js";
import { Camera } from "three";
const scene = new THREE.Scene();



const playerCar = Car();
//scene.add(playerCar);

var shipModel;
var camera;
var shipPos = new THREE.Vector3(0, 0, 0);
var directionVec = new THREE.Vector3(0, -50, 100);

function loadPlayerShip() {
  const loader = new GLTFLoader();

  loader.load(
    "./assets/ship1/scene.gltf",
    function (gltf) {
      
      shipModel = gltf;
      
      shipModel.scene.scale.set(0.1, 0.1, 0.1);
      shipModel.scene.position.set(0, -1, 0);

      scene.add(shipModel.scene);
      document.addEventListener("keydown", onDocumentKeyDown, false);

      function onDocumentKeyDown(event) {
        var xSpeed = 0.5;
        var zSpeed = 0.5;

        var keyCode = event.which;
        if (keyCode == 87) {
          shipModel.scene.position.z += zSpeed;
          shipPos = shipModel.scene.position;       
        } else if (keyCode == 83) {
          shipModel.scene.position.z -= zSpeed;
          shipPos = shipModel.scene.position; 
        } else if (keyCode == 65) {
          shipModel.scene.position.x += xSpeed;
          shipPos = shipModel.scene.position; 
        } else if (keyCode == 68) {
          shipModel.scene.position.x -= xSpeed;
          shipPos = shipModel.scene.position; 
        } else if (keyCode == 32) {
          shipPos.set(0, 0, 0);
        }
      }

    },
    undefined,
    function (error) {
      console.error(error);
    }
  );
}

loadPlayerShip();

function resetScene(canvas) {
  const renderer = buildRenderer(canvas);

  camera = new THREE.PerspectiveCamera(
    55,
    window.innerWidth / window.innerHeight,
    1,
    20000
  );

  camera.position.set(0, 100, -100);
  camera.lookAt(0, 1, 100);

  //camera.lookAt(shipModel.scene.position);
  // const controls = new OrbitControls(camera, renderer.domElement);
  // controls.maxPolarAngle = Math.PI * 0.495;
  // controls.target.set(0, 0, 0);
  // controls.minDistance = 40.0;
  // controls.maxDistance = 200.0;
  // controls.update();

  const sky = buildSky();
  const sun = buildSun();
  const water = buildWater();
  //const playerShip1 = loadPlayerShip();
  //scene.add(playerShip1.scene);

  const mainLight = new THREE.DirectionalLight("white", 1);
  mainLight.position.set(1, 1, 0);
  scene.add(mainLight);

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

    return sky;
  }

  function buildSun() {
    const pmremGenerator = new THREE.PMREMGenerator(renderer);
    const sun = new THREE.Vector3();

    // Defining the x, y and z value for our 3D Vector
    const theta = Math.PI * (0.49 - 0.5);
    const phi = 2 * Math.PI * (0.205 - 0.5);
    sun.x = Math.cos(phi);
    sun.y = Math.sin(phi) * Math.sin(theta);
    sun.z = Math.sin(phi) * Math.cos(theta);

    sun.x = 1;
    sun.y = 0;
    sun.z = 1;

    sky.material.uniforms["sunPosition"].value.copy(sun);
    //scene.environment = pmremGenerator.fromScene(sky).texture;
    return sun;
  }

  function buildWater() {
    const waterGeometry = new THREE.PlaneGeometry(10000, 10000);
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


  this.update = function () {
    water.material.uniforms["time"].value += 1.0 / 60.0;
    renderer.render(scene, camera);
    //camera.lookAt(shipModel.scene.position);
    camera.position.set(shipPos.x - directionVec.x, shipPos.y - directionVec.y, shipPos.z - directionVec.z);
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
const plswork = new resetScene(canvas);

function animate() {
  requestAnimationFrame(animate);
  plswork.update();
}

animate();
