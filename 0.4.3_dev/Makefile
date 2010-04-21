INSTALL_DIR=/Library/MonkeyWorks

all: req-libs scarab experiments core mwcocoa server core-plugins tools

clean: clean-req-libs uninstall-experiments clean-scarab clean-core clean-mwcocoa clean-server clean-core-plugins uninstall clean-tools

uninstall: clean-req-libs uninstall-experiments uninstall-scarab uninstall-core uninstall-mwcocoa uninstall-server uninstall-core-plugins uninstall-tools


FORCE:

# scarab
scarab: FORCE
	(cd Scarab; xcodebuild build -target libscarab.a -configuration Development)

clean-scarab: FORCE
	(cd Scarab; xcodebuild clean -alltargets -configuration Development)

uninstall-scarab: FORCE
	(cd Scarab; xcodebuild build -target ScarabUninstall -configuration Development)

# experiments
experiments: FORCE
	(cd MonkeyWorksExperimentXML; xcodebuild build -target InstallTests -configuration Release)

uninstall-experiments: FORCE
	(cd MonkeyWorksExperimentXML; xcodebuild build -target UninstallExperiments -configuration Release)

# MonkeyWorksCore
core: scarab req-libs
	(cd MonkeyWorksCore; xcodebuild build -target Everything -configuration Development)

clean-core: FORCE
	(cd MonkeyWorksCore; xcodebuild clean -alltargets -configuration Development)

uninstall-core: FORCE
	(cd MonkeyWorksCore; xcodebuild build -target CoreUninstall -configuration Development)

#required libraries
req-libs: FORCE
	(cd MonkeyWorksMacRequirements; make all)

clean-req-libs: FORCE
	(cd MonkeyWorksMacRequirements; make clean)

# Core Plugins
core-plugins: movie-plugin rectangle-plugin circle-plugin ITC18-plugin
clean-core-plugins: clean-movie-plugin clean-rectangle-plugin clean-circle-plugin clean-ITC18-plugin
uninstall-core-plugins: uninstall-movie-plugin uninstall-rectangle-plugin uninstall-circle-plugin uninstall-ITC18-plugin

#movie stimulus
movie-plugin: core
	(cd DLabPlugins/MoviePlugin; xcodebuild build -target MovieStimulusPlugin -configuration Debug)

clean-movie-plugin: FORCE
	(cd DLabPlugins/MoviePlugin; xcodebuild clean -alltargets -configuration Debug)

uninstall-movie-plugin: FORCE
	(cd DLabPlugins/MoviePlugin; xcodebuild build -target UninstallMovieStimulusPlugin -configuration Debug)


#rectangle stimulus
rectangle-plugin: core
	(cd DLabPlugins/RectangleStimulus; xcodebuild build -target RectangleStimulusPlugin -configuration Debug)

clean-rectangle-plugin: FORCE
	(cd DLabPlugins/RectangleStimulus; xcodebuild clean -alltargets -configuration Debug)

uninstall-rectangle-plugin: FORCE
	(cd DLabPlugins/RectangleStimulus; xcodebuild build -target UninstallRectangleStimulusPlugin -configuration Debug)

#circle stimulus
circle-plugin: core
	(cd DLabPlugins/CircleStimulus; xcodebuild build -target CircleStimulusPlugin -configuration Debug)

clean-circle-plugin: FORCE
	(cd DLabPlugins/CircleStimulus; xcodebuild clean -alltargets -configuration Debug)

uninstall-circle-plugin: FORCE
	(cd DLabPlugins/CircleStimulus; xcodebuild build -target UninstallCircleStimulusPlugin -configuration Debug)

#ITC18 stimulus
ITC18-plugin: core
	(cd DLabPlugins/ITC18Plugin; xcodebuild build -target ITC18Plugin -configuration Debug)

clean-ITC18-plugin: FORCE
	(cd DLabPlugins/ITC18Plugin; xcodebuild clean -alltargets -configuration Debug)

uninstall-ITC18-plugin: FORCE
	(cd DLabPlugins/ITC18Plugin; xcodebuild build -target UninstallITC18Plugin -configuration Debug)



#MonkeyWorksCocoa
mwcocoa: core scarab
	(cd MonkeyWorksCocoa; xcodebuild build -target MonkeyWorksCocoa -configuration Development)

clean-mwcocoa: FORCE
	(cd MonkeyWorksCocoa; xcodebuild clean -alltargets -configuration Development)

uninstall-mwcocoa: FORCE
	(cd MonkeyWorksCocoa; xcodebuild build -target UninstallMWCocoa -configuration Development)


#MonkeyWorksCocoa
server: core mwcocoa scarab
	(cd MonkeyWorksServer; xcodebuild build -target MonkeyWorksServer -configuration Development)

clean-server: FORCE
	(cd MonkeyWorksServer; xcodebuild clean -alltargets -configuration Development)

uninstall-server: FORCE
	(cd MonkeyWorksServer; xcodebuild build -target UninstallServer -configuration Development)


#tools
tools: dfindexer monkeyworks-stream-utilities matlab-data-reader data-file-reader
clean-tools: clean-dfindexer clean-monkeyworks-stream-utilities clean-matlab-data-reader clean-data-file-reader
uninstall-tools: uninstall-dfindexer uninstall-monkeyworks-stream-utilities uninstall-data-file-reader

dfindexer: core scarab
	(cd MonkeyWorksTools/DataFileIndexer; xcodebuild build -target dfindex -configuration Debug)

clean-dfindexer: FORCE
	(cd MonkeyWorksTools/DataFileIndexer; xcodebuild clean -alltargets -configuration Debug)

uninstall-dfindexer: FORCE
	(cd MonkeyWorksTools/DataFileIndexer; xcodebuild build -target dfindexerUninstall -configuration Debug)


monkeyworks-stream-utilities: core scarab dfindexer
	(cd MonkeyWorksTools/MonkeyWorksStreamUtilities; xcodebuild build -target libMonkeyWorksStreamUtilities -configuration Debug)

clean-monkeyworks-stream-utilities: FORCE
	(cd MonkeyWorksTools/MonkeyWorksStreamUtilities; xcodebuild clean -alltargets -configuration Debug)

uninstall-monkeyworks-stream-utilities: FORCE
	(cd MonkeyWorksTools/MonkeyWorksStreamUtilities; xcodebuild build -target UninstallMonkeyWorksStreamUtilities -configuration Debug)


matlab-data-reader: core scarab monkeyworks-stream-utilities dfindexer
	(cd MonkeyWorksTools/MatlabDataReader; make)

clean-matlab-data-reader: FORCE
	(cd MonkeyWorksTools/MatlabDataReader; make clean)


data-file-reader: core scarab
	(cd MonkeyWorksTools/DataFileReader; xcodebuild build -target mwdfr -configuration Debug)

clean-data-file-reader: FORCE
	(cd MonkeyWorksTools/DataFileReader; xcodebuild clean -alltargets -configuration Debug)

uninstall-data-file-reader: FORCE
	(cd MonkeyWorksTools/DataFileReader; xcodebuild build -target UninstallDataFileReader -configuration Debug)




