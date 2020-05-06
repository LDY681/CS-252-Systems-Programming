function updateProfile() {
    var name = document.getElementById("name").value;
    var email = document.getElementById("email").value;
    var phone = document.getElementById("phone").value;

    var user = firebase.auth().currentUser;

    var mydatabase = firebase.database().ref();
    var userProfile = mydatabase.child("Users");
    userProfile.push({
        "FullName": name,
        "Email": email,
        "PhoneNumber": phone
    });


    window.alert("Profile updated!")

}