#include "window.h"
#include <cmath>  // for sine stuff
#include <assert.h>

Window::Window() : plot( QString("Heart Rate Monitor") ), count(0) // <-- 'c++ initialisation list' - google it!
{

    button.setText("START RECORD");
    connect(&button, SIGNAL(released()), SLOT(Record()));

    button2.setText("FILTER ON");
    connect(&button2, SIGNAL(released()), SLOT(Filter()));



	// set up the initial plot data
	for( int index=0; index<plotDataSize; ++index )
	{
		xData[index] = index;
        yData[index] = 0;
	}



	// make a plot curve from the data and attach it to the plot
	curve.setSamples(xData, yData, plotDataSize);
	curve.attach(&plot);

	plot.replot();
	plot.show();

    hLayout.addWidget(&button);
    hLayout.addWidget(&button2);


    vLayout.addWidget(&plot);
    vLayout.addLayout(&hLayout);
    setLayout(&vLayout);



    reader = new ADCreader();
    reader->start();

    lp.setup (3, 250, 45); //order,sampling rate, Fc
    lp.reset ();
}


void Window::timerEvent( QTimerEvent * )
{

    while(reader->hasSample()){


            //Filter or not
            if (flag2 == 1)
                inVal = lp.filter(reader->getSample());
            else
                inVal = reader->getSample();


            memmove( yData, yData+1, (plotDataSize-1) * sizeof(double) );
            converted = (inVal/65536)*3.3;
            yData[plotDataSize-1] = converted;
            curve.setSamples(xData, yData, plotDataSize);
            ++count;

            //Save to text file if button pressed
            if (flag == 1){
                QTextStream out(&file);
                out<<count<<" "<<converted<<endl;

            }
    }
        plot.replot();

}


void Window::Record(){
    if (flag==0){
        button.setText("STOP RECORD");
        flag = 1;
        file.setFileName(filename);
        file.open(QIODevice::WriteOnly|QIODevice::Text);
        count = 0;
    }
    else{
        button.setText("START RECORD");
        flag = 0;
        file.close();
    }
}

void Window::Filter(){
    if (flag2 == 0){
        button2.setText("FILTER OFF");
        flag2 = 1;
    }
    else{
        button2.setText("FILTER ON");
        flag2 = 0;
    }

}