var firebaseConfig = { //firebase cdn code
    apiKey: "AIzaSyDLEco45i2BszWHJb67eAc7qx8OZ-d9vC8",
    authDomain: "iot-webapplication-47.firebaseapp.com",
    databaseURL: "https://iot-webapplication-47.firebaseio.com",
    projectId: "iot-webapplication-47",
    storageBucket: "iot-webapplication-47.appspot.com",
    messagingSenderId: "639567912933",
    appId: "1:639567912933:web:e8140919d9641904e2590c"
};
firebase.initializeApp(firebaseConfig);

var slotNumber=0;
site=localStorage.getItem('bedNumber');  //get the passed data

function onLoadFirebase() {
    var dtTemp = {
        vital: "-1"
    }
    var dtTemp1 = {
        sendBot: "-1"
    }
    var ref = database.ref("botCONTROL");
    ref.set(dtTemp);
    ref = database.ref("botCONTROL");
    ref.set(dtTemp1);
    
    var slotRef = firebase.database().ref('main/'+site+'/slot');
    slotRef.on("value", function(data, prevChildKey) {
       slotNumber = data.val();
       console.log("slot is: " + slotNumber);
       document.getElementById("slotDiplay").innerHTML = "CURRENT SELECTED SLOT: "+slotNumber;
    }); 
}


const medicine = document.getElementById('med');
const dosage = document.getElementById('dose');
const clockTime = document.getElementById('clockTime');
const form = document.getElementById('form');
const btn1 = document.getElementById('but1');

const btnSend = document.getElementById('send');
const btnDeliver = document.getElementById('deliver');
const btnReturn = document.getElementById('return');
const btnSlotSubmit = document.getElementById('slotSubmit');
//foreign code
document.getElementById("deliver").innerHTML = "COLLECT VITALS "+"("+site+")";
document.getElementById("send").innerHTML = "SEND BOT "+"("+site+")";
//foreign code

//var pathMain='alternate';
var pathMain='main/'+site+'/med/';

let container = document.querySelector('.container');
let ul = document.createElement('ul');
container.appendChild(ul);

var dummyItems = [];
var elementKeys = [];

const database = firebase.database();

var ref = database.ref(pathMain);
ref.on('value', gotData, errData);

function gotData(data) { //this function retrieves the data from firebase
    
    var entries = data.val();
    var keys = Object.keys(entries);
    //console.log(keys);
    
    ul = document.createElement('ul'); //reference the dom again
    container.appendChild(ul);

    for (var i = 0; i < keys.length; i++) {
        var k = keys[i];
        var medicine = entries[k].medicine;
        var dosage = entries[k].dosage;
        var meal = entries[k].meal;
        var timeClock = entries[k].time;

        var combine = 'MEDICINE: ' + medicine + ', DOSAGE: ' + dosage + ", MEAL: " + meal+", TIME: " + timeClock;
        console.log(combine);
        dummyItems.push(combine);
        //console.log(name,score);
    }

    var id = 0;

    dummyItems.forEach(function(item) { //for each array item insert them in the html list DOM
        var strID = 'btn' + '_' + id; //creates a new ID for every button
        id = id + 1;
        elementKeys = [];

        let li = document.createElement('li');
        let button = document.createElement('button');
        let str = document.createTextNode(item);

        li.appendChild(str);

        button.innerHTML = 'DELETE';
        //button.setAttribute("id", strID);  //sets an attribute for an elements
        //button.setAttribute("name", strID);
        li.appendChild(button);
        li.setAttribute('data-id', strID);

        ul.appendChild(li);

        // deleting data
        button.addEventListener('click', (e) => {
            e.stopPropagation();
            container.removeChild(ul); //remove the list before saving a new entry
            //so there wont be any duplicate values
            dummyItems = [];

            let id = e.target.parentElement.getAttribute('data-id');
            var res = id.split("_"); //btn_0 spliting in order to get the number only
            var order = res[1]; //res[0] has btn and res[1] has the number
            console.log(res[1]);

            var ref = firebase.database().ref(pathMain);
            ref.orderByKey().on("child_added", function(snapshot) {
                elementKeys.push(snapshot.key);
            });
            console.log(elementKeys[order]);
            var path = pathMain+ '/' + elementKeys[order];
            database.ref(path).remove();
        });

    });
    
    var slotRef = firebase.database().ref('main/'+site+'/slot');
    slotRef.on("value", function(data, prevChildKey) {
       slotNumber = data.val();
       console.log("slot: " + slotNumber);
       document.getElementById("slotDiplay").innerHTML = "CURRENT SELECTED SLOT: "+slotNumber;
    });

}

function errData(error) {
    console.log(error);
}

//saves the data
btn1.addEventListener('click', (e) => { //alternate saving
    e.preventDefault();
    console.log("workingg");
    
    
    if (dummyItems.length != 0)
        container.removeChild(ul); //remove the list before saving a new entry
    //so there wont be any duplicate values
    dummyItems = []; // also empty the array
    var ref = database.ref('/'+pathMain+'/');

    //radio buttons
    var temp_meal;
    var ele = document.getElementsByName('meal');
    for (var i = 0; i < ele.length; i++) {
        if (ele[i].checked) {
            console.log(ele[i].value);
            temp_meal = ele[i].value;
        }

    }

    var data = {
        medicine: medicine.value,
        dosage: dosage.value,
        meal: temp_meal,
        time: clockTime.value
    }

    ref.push(data);
    form.reset();
});



btnSend.addEventListener('click', (e) => { //alternate saving
    e.preventDefault();
    
    
    var strT=site.slice(11, site.length)
    
    var data = {
        sendBot: strT
    }
    e.preventDefault();
    console.log("sending");
    var ref = database.ref("botCONTROL");
    ref.set(data);
});

btnDeliver.addEventListener('click', (e) => { //alternate saving
    e.preventDefault();
    console.log("delivering");
    
    var strT=site.slice(11, site.length)
    
    var data = {
        vital: strT
    }
    e.preventDefault();
    console.log("sending");
    var ref = database.ref("botCONTROL");
    ref.set(data);
});

btnReturn.addEventListener('click', (e) => { //alternate saving
    e.preventDefault();
    var data = {
        sendBot: "0"
    }
    e.preventDefault();
    console.log("return");
    var ref = database.ref("botCONTROL");
    ref.set(data);
});


btnSlotSubmit.addEventListener('click', (e) => { //alternate saving
    e.preventDefault();
    console.log("workingg");
    
    const slotInput = document.getElementById('slotSelect');
    var pathToSave='main/'+site+'/slot/';
    var ref = database.ref('/'+pathToSave);

    console.log(slotInput.value);         
    ref.set(slotInput.value);
    form.reset();
    
    var slotRef = firebase.database().ref('main/'+site+'/slot');
    slotRef.on("value", function(data, prevChildKey) {
       slotNumber = data.val();
       console.log("slot: " + slotNumber);
       document.getElementById("slotDiplay").innerHTML = "CURRENT SELECTED SLOT: "+slotNumber;
    });
});


var values=[];
var maxValues=[];
var minValues=[];
var avgValues=[];
var variable=[];
var timeValues=[];
var from;
var counter=0;


$(document).ready(function() {
    var database = firebase.database();
    
var rootRef=firebase.database().ref().child("main/"+site+"/vitals/dateData/");
    rootRef.on("child_added", snap => {
        var childData = snap.val();
        //console.log(name);
        timeValues.push(childData);
        //$(".max").text("max: " + maxValues);
    });  

var rootRef=firebase.database().ref().child("main/"+site+"/vitals/heartRate/");
    rootRef.on("child_added", snap => {
        var childData = snap.val();
        //console.log(name);
        minValues.push(childData);
       // $(".min").text("min: " + minValues);
        //
        var ctx = document.getElementById('chartOfHeartRate').getContext('2d');
            var myChart = new Chart(ctx, { //change should go here
                type: 'line',
                data: {
                    labels: timeValues, //x axis value
                    datasets: [
                        {
                           label: 'HEART RATE READINGS', //y axis value's label
                        data: minValues, //y axis values [can be put an array]
                        backgroundColor: [
                              //color of avg curve
                            'rgba(52, 235, 85, 0.2)'
                        ],
                        borderColor: [
                            'rgba(54, 162, 235, 1)',
                            'rgba(255, 162, 235, 1)',
                            'rgba(255, 206, 86, 1)',
                            'rgba(75, 192, 192, 1)',
                            'rgba(153, 102, 255, 1)',
                            'rgba(255, 159, 64, 1)'
                        ],
                        borderWidth: 1 
                        }       
                              ]
                },
                options: {
                    animation: false,
                    scales: {
                        yAxes: [{
                            ticks: {
                                beginAtZero: true
                            }
                        }]
                    }
                }
            });
    });  
    var rootRef=firebase.database().ref().child("main/"+site+"/vitals/temperature/");
    rootRef.on("child_added", snap => {
        var childData = snap.val();
        //console.log(name);
        maxValues.push(childData);
       // $(".min").text("min: " + minValues);
        //
        var ctx = document.getElementById('chartOfTemperature').getContext('2d');
            var myChart = new Chart(ctx, { //change should go here
                type: 'line',
                data: {
                    labels: timeValues, //x axis value
                    datasets: [
                        {
                           label: 'TEMPERATURE READINGS', //y axis value's label
                        data: maxValues, //y axis values [can be put an array]
                        backgroundColor: [
                              //color of avg curve
                            'rgba(3, 240, 252, 0.2)'
                        ],
                        borderColor: [
                            'rgba(54, 162, 235, 1)',
                            'rgba(255, 162, 235, 1)',
                            'rgba(255, 206, 86, 1)',
                            'rgba(75, 192, 192, 1)',
                            'rgba(153, 102, 255, 1)',
                            'rgba(255, 159, 64, 1)'
                        ],
                        borderWidth: 1 
                        }       
                              ]
                },
                options: {
                    animation: false,
                    scales: {
                        yAxes: [{
                            ticks: {
                                beginAtZero: true
                            }
                        }]
                    }
                }
            });
    }); 
});

