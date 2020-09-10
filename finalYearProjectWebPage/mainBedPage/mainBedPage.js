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

function onLoadFirebase() {
    var dtTemp = {
        deliverGood: "-1"
    }
    var dtTemp1 = {
        sendBot: "-1"
    }
    var ref = database.ref("botCONTROL");
    ref.set(dtTemp);
    ref = database.ref("botCONTROL");
    ref.set(dtTemp1);
}

let container = document.querySelector('.container');
let ul = document.createElement('ul');
container.appendChild(ul);

var dummyItems = [];
var elementKeys = [];

const database = firebase.database();

var ref = database.ref('main');
ref.on('value', gotData, errData);

function gotData(data) { //this function retrieves the data from firebase
    var entries = data.val();
    var keys = Object.keys(entries);
    //console.log(keys);

    ul = document.createElement('ul'); //reference the dom again
    container.appendChild(ul);

    for (var i = 0; i < keys.length; i++) {
        var k = keys[i];
        console.log(k);
        dummyItems.push(k);
        //console.log(name,score);
    }

    var id = 0;

    dummyItems.forEach(function(item) { //for each array item insert them in the html list DOM
        var strID = item; //creates a new ID for every button
        id = id + 1;
        elementKeys = [];

        let li = document.createElement('li');
        let button = document.createElement('button');
        let str = document.createTextNode(item);

        li.appendChild(str);

        button.innerHTML = 'DASHBOARD';
        //button.setAttribute("id", strID);  //sets an attribute for an elements
        //button.setAttribute("name", strID);
        li.appendChild(button);
        li.setAttribute('data-id', strID);

        ul.appendChild(li);

        //dashboard visit
        button.addEventListener('click', (e) => {
            e.stopPropagation();
            //so there wont be any duplicate values
            dummyItems = [];

            let id = e.target.parentElement.getAttribute('data-id');
            //var res = id.split("_"); //btn_0 spliting in order to get the number only
            console.log(id); 
           localStorage.setItem('bedNumber',id);
           window.location='file:///C:/Users/shaikh/Desktop/web%20eng/nodemcu/working_but_do_test_only_here/finalYearProjectWebPage/medicinePage/saveData.html?meal=Before+meal';
        });

    });

}

function errData(error) {
    console.log(error);
}


//const some = document.querySelector('#form');
//console.log(some.textContent);