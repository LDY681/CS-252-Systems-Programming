
function loginUser(userType) {
  var mail;
  var pass;
  if(userType == "deliv"){
    mail = "demail";
    pass = "dpass";
  } else {
    mail = "remail";
    pass = "rpass";
  }
  var userEmail = document.getElementById(mail).value;
  var userPass = document.getElementById(pass).value;
  var change = true;
  firebase.auth().signInWithEmailAndPassword(userEmail, userPass).catch(function(error) {
    var errorCode = error.code;
    change = false;
    var errorMessage = error.message;
    if(errorMessage != "A network error (such as timeout, interrupted connection or unreachable host) has occurred."){
      window.alert("Error: " + errorMessage);
    }
    signOutUser();
  });
  if(change == true)
    changePage(userType);
}

function changePage(userType) {
  firebase.auth().onAuthStateChanged(function(user) {
    if (user) {
      // User is signed in.
      console.log(user.email);
      if(userType == "deliv"){
        window.location.href = ("driver.html");
      } else {
        window.location.href = ("restaurant.html");
      }
      
    } else {
      
    }
   });
 }

function createUser(userType) {
  var mail;
  var pass;
  if(userType == "deliv"){
    mail = "demail";
    pass = "dpass";
  } else {
    mail = "remail";
    pass = "rpass";
  }
  var userEmail = document.getElementById(mail).value;
  var userPass = document.getElementById(pass).value;

  firebase.auth().createUserWithEmailAndPassword(userEmail, userPass).catch(function(error) {
    var errorCode = error.code;
    var errorMessage = error.message;

    if(errorMessage != "A network error (such as timeout, interrupted connection or unreachable host) has occurred."){
      window.alert("Error: " + errorMessage);
    }
  });
  window.alert("Account created for " + userEmail);
  changePage(userType);
}


function signOutUser() {

  firebase.auth().signOut().then(function() {
    // Sign-out successful.
    window.location.href="index.html";
  }).catch(function(error) {
    // An error happened.
    window.alert("Error: " + error.message);
  });

}

