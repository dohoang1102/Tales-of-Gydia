if ! dpkg-query -W libsdl1.2-dev, then
	echo -n Install required package libsdl1.2-dev? \(y/n\)
	read $reply
	if ($reply == "y"); then
		sudo apt-get install libsdl1.2-dev
	else
		echo Build stopped.
		exit
	fi
fi

if ! dpkg-query -W libsdl-image1.2-dev, then
	echo -n Install required package libsdl-image1.2-dev? \(y/n\)
	read $reply
	if ($reply == "y"); then
		sudo apt-get install libsdl-image1.2-dev
	else
		echo Build stopped.
		exit
	fi
fi

if ! dpkg-query -W libsdl-ttf1.2-dev, then
	echo -n Install required package libsdl-ttf1.2-dev? \(y/n\)
	read $reply
	if ($reply == "y"); then
		sudo apt-get install libsdl-ttf1.2-dev
	else
		echo Build stopped.
		exit
	fi
fi

if ! dpkg-query -W libsdl-gfx1.2-dev, then
	echo -n Install required package libsdl1.2-dev? \(y/n\)
	read $reply
	if ($reply == "y"); then
		sudo apt-get install libsdl-gfx1.2-dev
	else
		echo Build stopped.
		exit
	fi
fi

echo All required packages are installed; now building...
g++ -o ToG test.cpp -w -lSDLmain -lSDL -lSDL_image -lSDL_ttf -lSDL_gfx &&
echo Build complete. Run ./ToG to play