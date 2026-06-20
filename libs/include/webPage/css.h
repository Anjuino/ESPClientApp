const char css[] PROGMEM = R"=(
body {
   font-family: sans-serif;
   text-align: center; 
   margin: 0;
   padding: 20px;
   background-color: #080808;
}

h1 {
   color: #ffffff;
   margin-bottom: 20px;
}

form, .ota-section {
   max-width: 400px; 
   margin: 0 auto; 
   padding: 20px;
   background-color: #fff;
   border-radius: 5px;
   box-shadow: 0px 0px 20px 9px rgb(0 80 136);
}

.ota-section {
   margin-top: 30px;
   padding-top: 20px;
}

.ota-section h3 {
   color: #333;
   margin-bottom: 15px;
   font-size: 1.17em;
}

label {
   display: block;
   margin-bottom: 5px;
   font-weight: bold;
   text-align: left;
}

input[type="text"],
input[type="password"],
select,
input[type="file"] {
   width: 100%;
   padding: 10px;
   margin-bottom: 15px;
   border: 1px solid #ccc;
   border-radius: 4px;
   box-sizing: border-box;
   background-color: #fff;
}

input[type="file"] {
   padding: 10px;
   cursor: pointer;
}

input[type="file"]::file-selector-button {
   background-color: #005088;
   color: white;
   padding: 8px 16px;
   border: none;
   border-radius: 4px;
   cursor: pointer;
   margin-right: 10px;
}

input[type="file"]::file-selector-button:hover {
   background-color: #bd0000;
}

button, .ota-button {
   background-color: #005088;
   color: white;
   padding: 12px 20px;
   border: none;
   border-radius: 4px;
   cursor: pointer;
   font-size: 16px;
   display: block; 
   margin: 25px auto;
   width: 100%;
   max-width: 200px;
   transition: background-color 0.3s ease;
}

button:hover, .ota-button:hover {
   background-color: #bd0000;
}

#otaStatus {
   margin-top: 10px;
   padding: 10px;
   border-radius: 4px;
   font-weight: bold;
   display: block;
}

#content {
   display: none; 
}

#loading {
   color: white;
   display: block;
   text-align: center;
   margin-top: 50px;
   font-size: 35px;
}

/* Разделитель в блоке OTA */
.ota-divider {
   border-top: 2px solid #e0e0e0;
   margin: 0 0 20px 0;
   padding: 0;
}

@media screen and (max-width: 980px ) {
   body {
      font-size: 20px;
      padding-top: 30%;
   }

   button, .ota-button {
      font-size: 40px;
      margin-top: 50px;
      width: 100%;
      max-width: 300px;
      padding: 20px;
   }

   form, .ota-section {
      font-size: 30px;
      max-width: 600px;
      padding: 30px;
   }

   input[type="text"],
   input[type="password"],
   select,
   input[type="file"] {
      font-size: 40px;
      padding: 15px;
   }

   label {
      font-size: 40px;
   }

   input[type="file"]::file-selector-button {
      font-size: 30px;
      padding: 15px 25px;
   }
}
)=";