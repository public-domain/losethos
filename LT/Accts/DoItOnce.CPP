//This file is executed by the
//first terminal window upon start-up.
//See $LK,"DoItOnce","FF:HOME/Adam3fSys.APZ,DoItOnce"$.

//It goes in your HOME directory and
//you can customize it.

//Delete this to get rid of
//default start-up msg.
Type("::/LT/Doc/Customize.TXZ");

"\nTake Tour";
if (YorN) {
  Cd("/LT/Misc/Tour");
  AutoFile("Tour");
}
