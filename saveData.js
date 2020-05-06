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



const medicine = document.getElementById('med');
const dosage = document.getElementById('dose');
const clockTime = document.getElementById('clockTime');
const form = document.getElementById('form');
const btn1 = document.getElementById('but1');


let container = document.querySelector('.container');
let ul = document.createElement('ul');
container.appendChild(ul);

var dummyItems = [];
var elementKeys = [];

const database = firebase.database();

var ref = database.ref('alternate');
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

        var combine = 'MEDICINE: ' + medicine + ' DOSAGE: ' + dosage + " MEAL: " + meal+" TIME: " + timeClock;
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

            var ref = firebase.database().ref("alternate");
            ref.orderByKey().on("child_added", function(snapshot) {
                elementKeys.push(snapshot.key);
            });
            console.log(elementKeys[order]);
            var path = 'alternate/' + elementKeys[order];
            database.ref(path).remove();
        });

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
    var ref = database.ref('/alternate/');

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

//const some = document.querySelector('#form');
//console.log(some.textContent);