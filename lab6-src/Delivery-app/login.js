function loginUser() {
  var userEmail = document.getElementById("email").value;
  var userPass = document.getElementById("pwd").value;

  firebase.auth().signInWithEmailAndPassword(userEmail, userPass).catch(function(error) {
    var errorCode = error.code;
    var errorMessage = error.message;
    window.alert("Error: " + errorMessage);
  });

}

function changePage() {
  firebase.auth().onAuthStateChanged(function(user) {
    if (user) {
      // User is signed in.
        // changed to home page
      window.location.href = ("home.html");
    } else {
      // No user is signed in.
    }
   });
 }

function createUser() {
  var userEmail = document.getElementById("email").value;
  var userPass = document.getElementById("pwd").value;

  firebase.auth().createUserWithEmailAndPassword(userEmail, userPass).catch(function(error) {
    var errorCode = error.code;
    var errorMessage = error.message;
    window.alert("Error: " + errorMessage);
  });
  window.alert("Account created for " + userEmail);
}
