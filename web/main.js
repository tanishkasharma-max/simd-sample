import createBrightnessModule from '../build/brightness.js';

let Module = null;

// Load WASM module
(async () => {
  Module = await createBrightnessModule({
    locateFile: (path) =>
      path.endsWith('.wasm')
        ? new URL(`../build/${path}`, import.meta.url).href
        : path,
  });

  console.log("WASM Loaded");
})();

// DOM elements
const fileInput = document.getElementById('file');
const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d');
const deltaSlider = document.getElementById('delta');
const applyBtn = document.getElementById('apply');
const dval = document.getElementById('dval');

let imageData = null;
let imgW = 0, imgH = 0;

deltaSlider.oninput = () => {
  dval.textContent = deltaSlider.value;
};

// Load selected image
fileInput.addEventListener('change', async (e) => {
  const file = e.target.files[0];
  if (!file) return;

  const img = new Image();
  img.src = URL.createObjectURL(file);
  await img.decode();

  // Always use 500x500
  canvas.width = 500;
  canvas.height = 500;

  // Stretch-draw
  ctx.drawImage(img, 0, 0, 500, 500);

  imageData = ctx.getImageData(0, 0, 500, 500);
  imgW = 500;
  imgH = 500;

  console.log("Image loaded at 500x500");
});

// Apply brightness button click
applyBtn.addEventListener('click', () => {
  if (!imageData) return alert("Load an image first");
  if (!Module) return alert("WASM not loaded yet");

  const delta = parseInt(deltaSlider.value, 10);

  const bufSize = imageData.data.length;

  // Allocate memory in WASM
  const ptr = Module._malloc(bufSize);

  // Copy input pixels → WASM memory
  Module.HEAPU8.set(imageData.data, ptr);


  const start = performance.now();

  // Call C++ function
  Module.ccall(
    'increase_brightness',
    'void',
    ['number', 'number', 'number', 'number', 'number'],
    [ptr, imgW, imgH, imgW * 4, delta]
  );

   const end = performance.now();
    console.log(`Brightness processing time: ${(end - start).toFixed(2)} ms`);
  // Copy output back to JS
  const out = new Uint8ClampedArray(Module.HEAPU8.buffer, ptr, bufSize);
  imageData.data.set(out);

  // Draw result
  ctx.putImageData(imageData, 0, 0);

  // Free memory
  Module._free(ptr);

  console.log("Brightness applied:", delta);
});
