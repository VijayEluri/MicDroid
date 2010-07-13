/* MicApplication.java

   Copyright (c) 2010 Ethan Chen

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
 */

package com.intervigil.micdroid;

import java.io.File;

import android.app.Application;
import android.os.Environment;

public class MicApplication extends Application {

	@Override
	public void onCreate() {
		super.onCreate();
		File outputDir = new File(this.getOutputDirectory());
        if (!outputDir.exists()) {
        	outputDir.mkdir();
        }
        File libraryDir = new File(this.getLibraryDirectory());
        if (!libraryDir.exists()) {
        	libraryDir.mkdir();
        }
	}
	
	@Override
	public void onTerminate() {
		super.onTerminate();
	}
	
	private String getApplicationDirectory() {
		return Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "Android" + File.separator + "data" + File.separator + getPackageName();
	}
	
	public String getOutputDirectory() {
		return getApplicationDirectory() + File.separator + "temp";
	}
	
	public String getLibraryDirectory() {
		return getApplicationDirectory() + File.separator + "library";
	}
}
