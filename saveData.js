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



const fname = document.getElementById('fname');
const lname = document.getElementById('lname');

const btn1 = document.getElementById('but1');


let container = document.querySelector('.container');
let ul=document.createElement('ul');
container.appendChild(ul);

var dummyItems=[];
var elementKeys=[];

const database = firebase.database();

var ref = database.ref('alternate');
ref.on('value', gotData,errData);

function gotData(data){   //this function retrieves the data from firebase
  var entries=data.val();
  var keys = Object.keys(entries);
  //console.log(keys);
  
  ul=document.createElement('ul');  //reference the dom again
  container.appendChild(ul);  

  for (var i = 0; i < keys.length; i++) {
    var k=keys[i];
    var name=entries[k].name;
    var score=entries[k].score;

    var combine='FIRST NAME: ' + name + '\n' + ' LAST NAME: ' + score;
    console.log(combine);
    dummyItems.push(combine);
    //console.log(name,score);
  }

var id=0;

dummyItems.forEach(function(item){  //for each array item insert them in the html list DOM
    var strID='btn'+'_'+id; //creates a new ID for every button
    id=id+1;
    elementKeys=[];

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
        container.removeChild(ul);  //remove the list before saving a new entry
                                //so there wont be any duplicate values
        dummyItems=[]; 


        let id = e.target.parentElement.getAttribute('data-id');
        var res = id.split("_");
        var order=res[1];
        console.log(res[1]);

        var ref = firebase.database().ref("alternate");
        ref.orderByKey().on("child_added", function(snapshot) {
          elementKeys.push(snapshot.key);
        });
        console.log(elementKeys[order]);
        var path='alternate/'+elementKeys[order];
        database.ref(path).remove();
    });

});

}

function errData(error){
  console.log(error);
}


btn1.addEventListener('click', (e) => {   //alternate saving
    e.preventDefault();
    console.log("workingg");

    if(dummyItems.length!=0)
        container.removeChild(ul);  //remove the list before saving a new entry
                                //so there wont be any duplicate values
    dummyItems=[];               // also empty the array
    var ref=database.ref('/alternate/');

    var data={
        name: fname.value,
        score: lname.value
    }

    ref.push(data);
    
});

const some = document.querySelector('#form');
console.log(some.textContent);



