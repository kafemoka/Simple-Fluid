#ifndef TINYCL_H
#define TINYCL_H

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

namespace tcl {
	//I should just remove these..
	enum DEVICE { CPU = CL_DEVICE_TYPE_CPU, GPU = CL_DEVICE_TYPE_GPU };
	enum MEM { READ_ONLY = CL_MEM_READ_ONLY, WRITE_ONLY = CL_MEM_WRITE_ONLY,
		READ_WRITE = CL_MEM_READ_WRITE };

	/*
	* A lightweight class for simplifying some operations with OpenCL contexts
	* the platform, device context and queue are all public as well
	* to avoid getting in the way, or having to write too many pointless 
	* forwarding/wrapper functions
	*/
	class Context {
	public:
		/*
		* Create a new context to run on the device type specified.
		* Since this is kind of a lazy implementation and only meant for my own
		* use the first available device of the desired type will be chosen
		* perhaps later I'll add some desired properties that can be looked for
		* @param dev Device type to try and get
		* @param interop If we want OpenGL interop
		* @param profile If we want profiling enabled in the OpenCL context
		*/
		Context(DEVICE dev, bool interop, bool profile);
		/*
		* Load a program from the file for use
		*/
		cl::Program loadProgram(const std::string &file);
		/*
		* Create a buffer of some desired size and pass some data to it
		* @param mem Type of memory we want to create
		* @param size Size of buffer to allocate
		* @param data The data to write to the buffer, nullptr indicates no data to write
		* @param offset The offset in the buffer to write at, default 0
		* @param blocking If this call should be blocking, default non-blocking
		* @param depends Events this operation depends on, default none
		* @param notify Event that this operation should notify upon completion, default none
		*/
		cl::Buffer buffer(int mem, size_t size, const void *data, size_t offset = 0, bool blocking = false,
			const std::vector<cl::Event> *depends = nullptr, cl::Event *notify = nullptr);
		/*
		* Create a buffer to make use of an existing OpenGL buffer for data
		* Note: Interop context is required!
		* @param mem Type of memory we want to create
		* @param buf The GL buffer we'll be using for storage
		*/
		cl::BufferGL bufferGL(int mem, GLuint buf);
		/*
		* Create an image that makes use of an existing GL texture for data
		* Note: Interop context is required!
		* @param mem Type of memory we want
		* @param tex The GL texture we're using
		*/
#ifdef CL_VERSION_1_2
		cl::ImageGL imageGL(int mem, GLuint tex);
#else
		cl::Image2DGL imageGL(int mem, GLuint tex);
#endif
		/*
		* Write some data to a buffer
		* @param buf The buffer to write too
		* @param size Size of data to write
		* @param data The data to write
		* @param offset Offset in the buffer to write too, default 0
		* @param blocking If this call should be blocking, default non-blocking
		* @param depends Events this operation depends on, default none
		* @param notify Event that this operation should notify upon completion, default none
		*/
		void writeData(cl::Buffer &buf, size_t size, const void *data, size_t offset = 0, bool blocking = false,
			const std::vector<cl::Event> *depends = nullptr, cl::Event *notify = nullptr);
		/*
		* Read some data from the buffer into the host memory
		* @param buf The buffer to read from
		* @param size Size of the data to read
		* @param data Host memory to write too
		* @param offset Offset in the buffer to start reading from
		* @param blocking If this call should be blocking
		* @param depends Events this operation depends on
		* @param notify Event that this operation should notify upon completion
		*/
		void readData(const cl::Buffer &buf, size_t size, void *data, size_t offset,
			bool blocking, const std::vector<cl::Event> *depends = nullptr, 
			cl::Event *notify = nullptr);
		/*
		* Run the desired kernel
		* @param kernel Kernel to run
		* @param global Global group dimensions
		* @param local Local group dimensions
		* @param offset Group dimension # offset
		* @param blocking If this call should be blocking, default false
		* @param depends Events this operation depends on
		* @param notify Event that this operation should notify upon completion
		*/
		void runNDKernel(cl::Kernel &kernel, cl::NDRange global, cl::NDRange local,
			cl::NDRange offset, bool blocking = false, const std::vector<cl::Event> *depends = nullptr,
			cl::Event *notify = nullptr);

	private:
		/*
		* Select the device to be used and setup the context and command queue
		* @param dev Device type to get
		* @param profile If we want profiling info available
		*/
		void selectDevice(DEVICE dev, bool profile);
		/*
		* Selecte the device to be used and setup the context and command queue for 
		* an OpenGL interop context
		* @param dev Device type to get
		* @param profile If we want profiling info available
		*/
		void selectInteropDevice(DEVICE dev, bool profile);

	public:
		std::vector<cl::Platform> mPlatforms;
		std::vector<cl::Device> mDevices;
		cl::Context mContext;
		cl::CommandQueue mQueue;
	};
}


#endif
