users
    lance:"{"Username":"lance","Email":"lance@mail.com","Password":"abc"}"

sensors
    humidity: 90        <--this is an example-->
    temperature: 30.5   <--this is an example-->
    status: locked/unlocked


commands
    request: ""add_key""                 <--this is an example-->

    <--the commands will be-->
    add_key                 <--this will reflect on lcd prompting users what type of key-->
        A - Pin
        B - Key
        C - Card
        D - Cancels Request

    remove_key               <--this will reflect on lcd prompting users what uid of passkey-->
        once uid is entered
        it will check it on firebase and delete that if it exists

    lock <--this command is sent by mit app-->
    unlock <--this command is sent by mit app-->
    once mechanism is done clear all commands

passkeys
    id: "{"UID":"id","Passkey":"76532","Key_Type":"PIN"}" 
    
    <--id can use numbers so if no numbers it would be like this-->
    001: "{"UID":"001","Passkey":"76532","Key_Type":"PIN"}"
    002: "{"UID":"002","Passkey":"765HA AHAH AHBH","Key_Type":"Card"}"
    003: "{"UID":"003","Passkey":"765HA JHAD AKNM","Key_Type":"Key"}"

    <--so if id's get deleted it would be like this it would be like this-->
    001: "{"UID":"001","Passkey":"76532","Key_Type":"PIN"}" 
    003: "{"UID":"003","Passkey":"765HA JHAD AKNM","Key_Type":"Key"}"

