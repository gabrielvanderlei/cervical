 // Initialize Firebase
 var config = {
    apiKey: "AIzaSyB-puOXLaceDzkGmcg_7CqqkIaFPSitRzU",
    authDomain: "cervical-1a344.firebaseapp.com",
    databaseURL: "https://cervical-1a344.firebaseio.com",
    projectId: "cervical-1a344",
    storageBucket: "",
    messagingSenderId: "190589431562"
  };
  firebase.initializeApp(config);
  // Initialize Cloud Firestore through Firebase
    var db = firebase.firestore();
    
  function criarUsuario(){
    firebase.auth().createUserWithEmailAndPassword(document.getElementById('emailc').value,document.getElementById('senhac').value)
      .then(function(data){
alert('Registrado');
    })
      .catch(function(error) {
      // Handle Errors here.
      var errorCode = error.code;
      var errorMessage = error.message;
      alert(errorMessage)
    });
  }
  var uuid;
  function logarUsuario(){
     
 firebase.auth().signInWithEmailAndPassword(document.getElementById('emaill').value,document.getElementById('senhal').value)
        .catch(function(error) {
      // Handle Errors here.
      var errorCode = error.code;
      var errorMessage = error.message;
      alert(errorMessage);
    });
    
  }
  
    function show(id){
      $('.section').css('display', 'none');
      $('#'+id).css('display','block');
    }
  
 function carregarSensores(doc){
var data = doc.data();
document.getElementById("sensores").innerHTML = data.sensores;
}
  firebase.auth().onAuthStateChanged(function(user) {
    if (user) {
      // User is signed in.
      var displayName = user.displayName;
      var email = user.email;
      var emailVerified = user.emailVerified;
      var photoURL = user.photoURL;
      var isAnonymous = user.isAnonymous;
      
uuid = user.uid;
      var providerData = user.providerData;
var user=db.collection("users").doc(uuid).get().then(carregarSensores)
.catch(function(){
        db.collection("users").doc(uuid).add({
            email: document.getElementById('emailc').value
        })
        .then(function(docRef) {
            alert('Usu�rio configurado');
        })
        .catch(function(error) {
            console.error("Error adding document: ", error);
        });
      // ...
});
      
      document.getElementById('username').innerHTML = email;
      
      show('painel');
carregarSensores();
    } else {
      // User is signed out.
      // ...
      show('main');
    }
  });
  
function addSensor(){
var user=db.collection("users").doc(uuid);
user.set({
sensores: $("#sensor").val()
}, {merge: true });
show("painel");
carregarSensores();
}
  function logOut(){
    firebase.auth().signOut().then(function() {
      // Sign-out successful.
      alert('Deslogado');
    }).catch(function(error) {
      // An error happened.
      alert('Algum erro ocorreu');
    });
  }
    
  