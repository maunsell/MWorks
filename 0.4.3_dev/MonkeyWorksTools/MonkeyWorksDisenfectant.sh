#!/bin/sh

# this script goes through and deletes all remnats of existing MonkeyWorks stuff

rm -rf /Library/MonkeyWorks

rm -rf /Applications/MonkeyWorksServer.app
rm -rf /Applications/MonkeyWorksClient.app
rm -rf /Applications/MonkeyWorkEditor.app
rm -rf /Applications/New\ Editor.app
rm -rf /Applications/NewEditor.app
rm -rf /Applications/NewClient.app
rm -rf /Applications/New\ Client.app
rm -rf /Applications/MWClient.app
rm -rf /Applications/MWEditor.app
rm -rf /Applications/MWServer.app

rm -rf /Library/Frameworks/MonkeyWorksCore.framework
rm -rf /Library/Frameworks/MonkeyWorksCocoa.framework
rm -rf /Library/Frameworks/ITC.framework
rm -rf /Library/Frameworks/libperl++.framework
rm -rf /Library/Frameworks/ABCApplication.framework
rm -rf /Library/Frameworks/ABCFoundation.framework
rm -rf /Library/Frameworks/Scarab.framework

rm -rf /Library/Application\ Support/MonkeyWorksClient
rm -rf /Library/Application\ Support/MonkeyWorksEditor
rm -rf /Library/Application\ Support/NewClient

rm -rf /Documents/MonkeyWorks
rm -rf  ~/Library/Preferences/edu.mit.MonkeyWorks*.plist
rm -rf  ~/Library/Preferences/edu.mit.MWClient.plist
rm -rf  ~/Library/Preferences/edu.mit.MWServer.plist
rm -rf  ~/Library/Preferences/edu.mit.MWEditor.plist
rm -rf  ~/Library/Preferences/org.coxlab.MWClient.plist
rm -rf  ~/Library/Preferences/org.coxlab.MWServer.plist
rm -rf  ~/Library/Preferences/org.coxlab.MWEditor.plist
rm -rf  ~/Library/Preferences/org.coxlab.New\ Client.plist
rm -rf  ~/Library/Preferences/org.coxlab.NewClient.plist
rm -rf  ~/Library/Preferences/org.coxlab.New\ Editor.plist
rm -rf  ~/Library/Preferences/org.coxlab.NewEditor.plist

