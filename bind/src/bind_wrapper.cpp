#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <Print.h>
#include <Stream.h>
#include <string>
#include <SerialMuxProtCommon.hpp>
#include <SerialMuxProtServer.hpp>

#ifndef MAX_CHANNELS
#define MAX_CHANNELS (10U)
#endif

namespace py = pybind11;

template<class PrintBase = Print>
class PyPrint : public PrintBase, public py::trampoline_self_life_support
{
public:
    /* Inherit the constructors */
    using PrintBase::PrintBase;

    /* Trampoline (need one for each virtual function) */
    void print(const char str[]) override
    {
        PYBIND11_OVERRIDE_PURE(void,      /* Return type */
                               PrintBase, /* Parent class */
                               print,     /* Name of function in C++ (must match Python name) */
                               str        /* Argument(s) */
        );
    }

    void print(uint8_t value) override
    {
        PYBIND11_OVERRIDE_PURE(void,      /* Return type */
                               PrintBase, /* Parent class */
                               print,     /* Name of function in C++ (must match Python name) */
                               value      /* Argument(s) */
        );
    }

    void print(uint16_t value) override
    {
        PYBIND11_OVERRIDE_PURE(void,      /* Return type */
                               PrintBase, /* Parent class */
                               print,     /* Name of function in C++ (must match Python name) */
                               value      /* Argument(s) */
        );
    }

    void print(uint32_t value) override
    {
        PYBIND11_OVERRIDE_PURE(void,      /* Return type */
                               PrintBase, /* Parent class */
                               print,     /* Name of function in C++ (must match Python name) */
                               value      /* Argument(s) */
        );
    }

    void print(int8_t value) override
    {
        PYBIND11_OVERRIDE_PURE(void,      /* Return type */
                               PrintBase, /* Parent class */
                               print,     /* Name of function in C++ (must match Python name) */
                               value      /* Argument(s) */
        );
    }

    void print(int16_t value) override
    {
        PYBIND11_OVERRIDE_PURE(void,      /* Return type */
                               PrintBase, /* Parent class */
                               print,     /* Name of function in C++ (must match Python name) */
                               value      /* Argument(s) */
        );
    }

    void print(int32_t value) override
    {
        PYBIND11_OVERRIDE_PURE(void,      /* Return type */
                               PrintBase, /* Parent class */
                               print,     /* Name of function in C++ (must match Python name) */
                               value      /* Argument(s) */
        );
    }

    /* Trampoline (need one for each virtual function) */
    void println(const char str[]) override
    {
        PYBIND11_OVERRIDE_PURE(void,      /* Return type */
                               PrintBase, /* Parent class */
                               println,   /* Name of function in C++ (must match Python name) */
                               str        /* Argument(s) */
        );
    }

    void println(uint8_t value) override
    {
        PYBIND11_OVERRIDE_PURE(void,      /* Return type */
                               PrintBase, /* Parent class */
                               println,   /* Name of function in C++ (must match Python name) */
                               value      /* Argument(s) */
        );
    }

    void println(uint16_t value) override
    {
        PYBIND11_OVERRIDE_PURE(void,      /* Return type */
                               PrintBase, /* Parent class */
                               println,   /* Name of function in C++ (must match Python name) */
                               value      /* Argument(s) */
        );
    }

    void println(uint32_t value) override
    {
        PYBIND11_OVERRIDE_PURE(void,      /* Return type */
                               PrintBase, /* Parent class */
                               println,   /* Name of function in C++ (must match Python name) */
                               value      /* Argument(s) */
        );
    }

    void println(int8_t value) override
    {
        PYBIND11_OVERRIDE_PURE(void,      /* Return type */
                               PrintBase, /* Parent class */
                               println,   /* Name of function in C++ (must match Python name) */
                               value      /* Argument(s) */
        );
    }

    void println(int16_t value) override
    {
        PYBIND11_OVERRIDE_PURE(void,      /* Return type */
                               PrintBase, /* Parent class */
                               println,   /* Name of function in C++ (must match Python name) */
                               value      /* Argument(s) */
        );
    }

    void println(int32_t value) override
    {
        PYBIND11_OVERRIDE_PURE(void,      /* Return type */
                               PrintBase, /* Parent class */
                               println,   /* Name of function in C++ (must match Python name) */
                               value      /* Argument(s) */
        );
    }

    size_t write(const uint8_t* buffer, size_t length) override
    {
        /* Must write the expanded version of the PYBIND11_OVERRIDE_PURE macro due to difficulties passing the buffer as
         * pointer. */

        /* Acquire the GIL while in this scope. */
        pybind11::gil_scoped_acquire gil;
        size_t                       writtenBytes = 0U;

        /* Try to look up the overridden method on the Python side.
         * Do not forget to static cast as this class will be derived from!
         */
        pybind11::function override = pybind11::get_override(static_cast<const PrintBase*>(this), "write");

        /* Method is found. */
        if (override)
        {
            /* Call method.

            Cast const char* to std::string,
            then cast std::string to py:bytes to send raw bytes. */
            auto obj = override(py::bytes(std::string(buffer, buffer + length)), length);

            /* Check if it returned a Python integer type. */
            if (py::isinstance<py::int_>(obj))
            {
                /* Cast it and assign it to the value. */
                writtenBytes = obj.cast<size_t>();
            }
        }
        else
        {
            /* Fail because it is a pure virtual function and shall be implemented. */
            pybind11::pybind11_fail("Tried to call pure virtual function \"Print::write\"");
        }

        return writtenBytes;
    }
};

template<class StreamBase = Stream>
class PyStream : public PyPrint<StreamBase>
{
public:
    /* Inherit the constructors */
    using PyPrint<StreamBase>::PyPrint;

    int available() const override
    {
        PYBIND11_OVERRIDE_PURE(int,        /* Return type */
                               StreamBase, /* Parent class */
                               available,  /* Name of function in C++ (must match Python name) */
        );
    };

    size_t readBytes(uint8_t* buffer, size_t length) override
    {
        /* Acquire the GIL while in this scope. */
        pybind11::gil_scoped_acquire gil;

        size_t readBytes = 0U;

        /* Try to look up the overridden method on the Python side. */
        pybind11::function override = pybind11::get_override(static_cast<const StreamBase*>(this), "readBytes");

        if (override)
        {
            /* Call the Python override.
             * Instead of asking Python to return data, we provide a writable memoryview
             * that references the C buffer so Python can write directly into it.
             */
            py::memoryview mv =
                py::memoryview::from_memory(reinterpret_cast<void*>(buffer), static_cast<py::ssize_t>(length));

            // Call the Python override with the memoryview and the requested length.
            // Python implementations MUST accept (mv, length) and write into mv.
            py::object obj = override(mv, length);

            /* If the Python function returned an integer, treat it as the number of
             * bytes read (no data copied). */
            if (py::isinstance<py::int_>(obj))
            {
                readBytes = obj.cast<size_t>();
            }
            else if (py::isinstance<py::bytes>(obj) || py::isinstance<py::bytearray>(obj) ||
                     py::isinstance<py::memoryview>(obj))
            {
                /* Obtain a buffer view of the returned object to copy into the C buffer. */
                py::buffer      buf  = py::buffer(obj);
                py::buffer_info info = buf.request();

                /* Ensure there is at least one dimension and a pointer. */
                if (info.ptr && info.size > 0)
                {
                    /* Determine how many bytes to copy: min(requested length, returned size in bytes).
                     * info.size is number of elements; itemsize gives bytes per element. We treat the
                     * buffer as raw bytes, so total_bytes = info.size * info.itemsize.
                     */
                    size_t total_bytes = static_cast<size_t>(info.size) * static_cast<size_t>(info.itemsize);
                    size_t to_copy     = (total_bytes < length) ? total_bytes : length;

                    memcpy(buffer, info.ptr, to_copy);
                    readBytes = to_copy;
                }
                else
                {
                    readBytes = 0U;
                }
            }
            else
            {
                /* Unsupported return type: raise a Python error. */
                PyErr_SetString(PyExc_TypeError, "readBytes override must return int or a bytes-like object");
                throw py::error_already_set();
            }
        }
        else
        {
            /* Fail because it is a pure virtual function and shall be implemented. */
            pybind11::pybind11_fail("Tried to call pure virtual function \"Stream::readBytes\"");
        }

        return readBytes;
    };
};

/** SerialMuxProt Server with fixed template argument. */
typedef SerialMuxProtServer<MAX_CHANNELS> SMPServer;

void test_printer(Print* printer)
{
    uint8_t testBuffer[10U] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};
    size_t  bytesToWrite    = 5U;

    printer->print("Hello");
    printer->print((uint8_t)1U);
    printer->print((uint16_t)2U);
    printer->print((uint32_t)3U);
    printer->print((int8_t)4);
    printer->print((int16_t)5);
    printer->print((int32_t)6);

    printer->println("World");
    printer->println((uint8_t)7U);
    printer->println((uint16_t)8U);
    printer->println((uint32_t)9U);
    printer->println((int8_t)10);
    printer->println((int16_t)11);
    printer->println((int32_t)12);

    printer->println("");

    if (bytesToWrite != printer->write(testBuffer, bytesToWrite))
    {
        printer->println("Return value was not as expected");
    }
    else
    {
        printer->println("All tests passed!");
    }
}

PYBIND11_MODULE(SerialMuxProt, m, py::mod_gil_not_used())
{
    py::class_<Print, PyPrint<>, py::smart_holder>(m, "Print")
        .def(py::init<>())
        .def("print", static_cast<void (Print::*)(const char[])>(&Print::print))
        .def("print", static_cast<void (Print::*)(uint8_t)>(&Print::print))
        .def("print", static_cast<void (Print::*)(uint16_t)>(&Print::print))
        .def("print", static_cast<void (Print::*)(uint32_t)>(&Print::print))
        .def("print", static_cast<void (Print::*)(int8_t)>(&Print::print))
        .def("print", static_cast<void (Print::*)(int16_t)>(&Print::print))
        .def("print", static_cast<void (Print::*)(int32_t)>(&Print::print))
        .def("println", static_cast<void (Print::*)(const char[])>(&Print::println))
        .def("println", static_cast<void (Print::*)(uint8_t)>(&Print::println))
        .def("println", static_cast<void (Print::*)(uint16_t)>(&Print::println))
        .def("println", static_cast<void (Print::*)(uint32_t)>(&Print::println))
        .def("println", static_cast<void (Print::*)(int8_t)>(&Print::println))
        .def("println", static_cast<void (Print::*)(int16_t)>(&Print::println))
        .def("println", static_cast<void (Print::*)(int32_t)>(&Print::println))
        .def("write", &Print::write);

    py::class_<Stream, Print, PyStream<>, py::smart_holder>(m, "Stream")
        .def(py::init<>())
        .def("available", &Stream::available)
        .def("readBytes", &Stream::readBytes);

    py::class_<SMPServer, py::smart_holder>(m, "SerialMuxProtServer")
        .def(py::init<Stream&>())
        .def(py::init<Stream&, void*>())
        .def("process", &SMPServer::process)
        // .def("send", static_cast<bool (SMPServer::*)(const char *channelName, const uint8_t *payload, uint8_t
        // payloadSize)>(&SMPServer::sendData)) .def("send", static_cast<void (SMPServer::*)(uint8_t channelNumber,
        // const void *payload, uint8_t payloadSize)>(&SMPServer::sendData))
        .def("getTxChannelNumber", &SMPServer::getTxChannelNumber)
        .def("createChannel", &SMPServer::createChannel)
        .def("subscribeToChannel", &SMPServer::subscribeToChannel)
        .def("isSynced", &SMPServer::isSynced)
        .def("getNumberOfTxChannels", &SMPServer::getNumberOfTxChannels)
        .def("getNumberOfRxChannels", &SMPServer::getNumberOfRxChannels)
        .def("registerOnSyncedCallback", &SMPServer::registerOnSyncedCallback)
        .def("registerOnDeSyncedCallback", &SMPServer::registerOnDeSyncedCallback)
        .def("setUserData", &SMPServer::setUserData);

    m.def("test_printer", &test_printer);
}