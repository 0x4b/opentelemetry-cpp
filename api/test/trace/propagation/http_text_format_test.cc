#include "opentelemetry/trace/propagation/http_text_format.h"
#include "opentelemetry/trace/propagation/http_trace_context.h"
#include "opentelemetry/context/context.h"
//#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/default_span.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/trace/trace_id.h"


#include <map>
#include <memory>
#include <string>

#include <gtest/gtest.h>

using namespace opentelemetry;

static nostd::string_view Getter(const std::map<std::string,std::string> &carrier, nostd::string_view trace_type = "traceparent") {
    auto it = carrier.find(std::string(trace_type));
    if (it != carrier.end()) {
        return nostd::string_view(it->second);
    }
    return "";
}

static void Setter(std::map<std::string,std::string> &carrier, nostd::string_view trace_type = "traceparent", nostd::string_view trace_description = "") {
    carrier[std::string(trace_type)] = std::string(trace_description);
}

static trace::propagation::HttpTraceContext<std::map<std::string,std::string>> format = trace::propagation::HttpTraceContext<std::map<std::string,std::string>>();

static nostd::string_view trace_id = "12345678901234567890123456789012";
static nostd::string_view span_id = "1234567890123456";

using map_http_trace_context = trace::propagation::HttpTraceContext<std::map<std::string,std::string>>;
TEST(HTTPTextFormatTest, TraceIdBufferGeneration)
{
    constexpr uint8_t buf[] = {1, 2, 3, 4, 5, 6, 7, 8, 8, 7, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    trace::TraceId id(buf);
    EXPECT_EQ(map_http_trace_context::GenerateTraceIdFromString("01020304050607080807aabbccddeeff"),id);
}

TEST(HTTPTextFormatTest, SpanIdBufferGeneration)
{
    constexpr uint8_t buf[] = {1, 2, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    trace::SpanId id(buf);
    EXPECT_EQ(map_http_trace_context::GenerateSpanIdFromString("0102aabbccddeeff"),id);
}

TEST(HTTPTextFormatTest, TraceFlagsBufferGeneration)
{
    trace::TraceFlags flags;
    EXPECT_EQ(map_http_trace_context::GenerateTraceFlagsFromString("00"),flags);
}

//TEST(HTTPTextFormatTest, TEST1)
//{
//    nostd::shared_ptr<trace::SpanContext> spc(new trace::SpanContext(
//        trace::propagation::HttpTraceContext<std::map<std::string,std::string>>::TraceId(GenerateTraceIdFromString("")),
//        trace::propagation::HttpTraceContext<std::map<std::string,std::string>>::SpanId(GenerateSpanIdFromString("")),
//        trace::propagation::HttpTraceContext<std::map<std::string,std::string>>::TraceFlags(GenerateTraceFlagsFromString("")),
//        ));
//}

TEST(HTTPTextFormatTest, NoSpanTest)
{
    const std::map<std::string,std::string> carrier = {{"traceparent","00-4bf92f3577b34da6a3ce929d0e0e4736-0102030405060708-01"},{"tracestate","congo=congosSecondPosition,rojo=rojosFirstPosition"}};
    nostd::shared_ptr<trace::SpanContext> spc{new trace::SpanContext()};
    context::Context ctx1 = context::Context("current-span",spc);
    context::Context ctx2 = format.Extract(Getter,carrier,ctx1);
    std::map<std::string,std::string> c2 = {};
    format.Inject(Setter,c2,ctx2);
    EXPECT_EQ(carrier.size(),c2.size());
}

//TEST(HTTPTextFormatTest, NoTraceParentHeader)
//{
//    // When trace context headers are not present, a new SpanContext
//    // should be created.
//    std::map<nostd::string_view,nostd::string_view> carrier = {};
//    trace::Span span = trace::propagation::GetCurrentSpan(format.Extract(Getter, carrier, Context()));
//    EXPECT_TRUE(span.GetContext() != NULL);
//}

//TEST(HTTPTextFormatTest, HeadersWithTraceState)
//{
//    // When there is a trace parent and trace state header, data from
//    // both should be added to the SpanContext.
//    nostd::string_view trace_parent_value = "00-"+ trace_id + "-" + span_id + "-00";
//    nostd::string_view trace_state_value = "foo=1,bar=2,baz=3";
//    std::map<nostd::string_view,nostd:string_view> carrier = { {"traceparent", trace_parent_value},
//                                                              {"tracestate", trace_state_value} };
//    trace::SpanContext span_context = trace::propagation::GetCurrentSpan(
//        format.Extract(
//            Getter,
//            carrier,
//            Context()
//        )
//    ).GetContext();
//    EXPECT_EQ(span_context.trace_id(), TraceId(nostd::span(trace_id,trace_id.length())));
//    EXPECT_EQ(span_context.span_id(), SpanId(nostd::span(span_id,span_id.length())));
//    EXPECT_EQ(span_context.trace_state(), {"foo": "1", "bar": "2", "baz": "3"}); // I am not certain how to create a trace state from map yet. Meeting with host hopefully to solve this
//    EXPECT_TRUE(span_context.HasRemoteParent());
//    std::map<nostd::string_view,nostd::string_view> carrier = {};
//    Span span = trace.DefaultSpan(span_context);
//
//    context::Context ctx = trace.SetSpanInContext(span,context::Context());
//    format.Inject(Setter, carrier, ctx);
//    EXPECT_EQ(carrier["traceparent"], traceparent_value);
//    int count = 0;
//    int start = -1;
//    int end = -1;
//    nostd::string_view trace_state_string = carrier["tracestate"];
//    for (int i = 0; i <= trace_state_string.length(); i++) {
//        if (trace_state_string[i] == ',' || i == trace_state_string.length()) {
//            count++;
//            nostd::string_view str = trace_state_string.substr(start,end);
//            start = -1;
//            end = -1;
//            EXPECT_TRUE(str == "foo=1" || str == "bar=2" || str == "baz=3");
//        } else {
//            if (trace_state_string[i] != ' ' && start == -1) {
//                start = i;
//            }
//            if (trace_state_string[i] != ' ') {
//                end = i;
//            }
//        }
//    }
//    EXPECT_EQ(count, 3);
//}
//
//TEST(HTTPTextFormatTest, InvalidTraceId)
//{
//    // If the trace id is invalid, we must ignore the full trace parent header,
//    // and return a random, valid trace.
//    // Also ignore any trace state.
//    std::map<nostd::string_view,nostd:string_view> carrier = { {"traceparent", "00-00000000000000000000000000000000-1234567890123456-00"},
//                                                               {"tracestate", "foo=1,bar=2,foo=3"} };
//    trace::Span span = trace::propagation::GetCurrentSpan(
//        format.Extract(
//            Getter,
//            carrier,
//            Context();
//        )
//    );
//    EXPECT_EQ(span.GetContext(), trace::SpanContext.GetInvalid());
//}
//
//TEST(HTTPTextFormatTest, InvalidParentId)
//{
//    // If the parent id is invalid, we must ignore the full trace parent
//    // header.
//    // Also ignore any trace state.
//    std::map<nostd::string_view,nostd:string_view> carrier = { {"traceparent", "00-00000000000000000000000000000000-0000000000000000-00"},
//                                                               {"tracestate", "foo=1,bar=2,foo=3"} };
//    trace::Span span = trace::propagation::GetCurrentSpan(
//        format.Extract(
//            Getter,
//            carrier,
//            Context()
//        )
//    );
//    EXPECT_EQ(span.GetContext(), trace::SpanContext.GetInvalid());
//}
//
//TEST(HTTPTextFormatTest, NoSendEmptyTraceState)
//{
//    // If the trace state is empty, do not set the header.
//    std::map<nostd::string_view,nostd:string_view> carrier = {};
//    trace::Span span = trace::DefaultSpan(
//        trace::SpanContext.Create(TraceId(nostd::span(trace_id,trace_id.length())),
//                                  SpanId(nostd::span(span_id,span_id.length())),
//                                  TraceFlags(nostd::span("00",2)), TraceState());
//    );
//    context::Context ctx = trace::propagation::SetSpanInContext(span);
//    format.Inject(Setter, carrier, ctx);
//    EXPECT_TRUE(carrier.count("traceparent") > 0);
//    EXPECT_FALSE(carrier.count("tracestate") > 0);
//}
//
//TEST(HTTPTextFormatTest, FormatNotSupported)
//{
//    // If the trace parent does not adhere to the supported format, discard it and
//    // create a new trace context.
//    std::map<nostd::string_view,nostd:string_view> carrier = { {"traceparent", "00-12345678901234567890123456789012-1234567890123456-00-residue"},
//                                                               {"tracestate", "foo=1,bar=2,foo=3"} };
//    span = trace.get_current_span(
//        format.Extract(
//            get_as_list,
//            carrier,
//            Context()
//        )
//    );
//    EXPECT_EQ(span.GetContext(), trace::SpanContext.GetInvalid());
//}
//
//TEST(HTTPTextFormatTest, PropagateInvalidContext)
//{
//    // Do not propagate invalid trace context.
//    std::map<nostd::string_view,nostd:string_view> carrier = {};
//    context::Context ctx = trace::propagation::SetSpanInContext(trace::DefaultSpan.GetInvalid());
//    format.Inject(Setter, carrier, ctx);
//    EXPECT_TRUE(carrier.count("traceparent") == 0);
//}
//
//TEST(HTTPTextFormatTest, TraceStateHeaderWithTrailingComma)
//{
//    // Do not propagate invalid trace context.
//    std::map<nostd::string_view,nostd:string_view> carrier = { {"traceparent", "00-12345678901234567890123456789012-1234567890123456-00"},
//                                                               {"tracestate", "foo=1,"} };
//    trace::Span span = trace::propagation::GetCurrentSpan(
//        format.Extract(
//            Getter,
//            carrier,
//            Context()
//        )
//    );
//    EXPECT_EQ(span.GetContext().trace_state()["foo"], "1");
//}
//
//TEST(HTTPTextFormatTest, TraceStateKeys)
//{
//    // Test for valid key patterns in the tracestate
//    nostd::string_view trace_state_value = "1a-2f@foo=bar1,1a-_*/2b@foo=bar2,foo=bar3,foo-_*/bar=bar4";
//    std::map<nostd::string_view,nostd:string_view> carrier = { {"traceparent", "00-12345678901234567890123456789012-1234567890123456-00"},
//                                                               {"tracestate", tracestate_value} };
//    trace::Span span = trace::propagation::GetCurrentSpan(
//        format.Extract(
//            Getter,
//            carrier,
//            Context()
//        )
//    );
//    EXPECT_EQ(span.GetContext().trace_state()["1a-2f@foo"], "bar1");
//    EXPECT_EQ(span.GetContext().trace_state()["1a-_*/2b@foo"], "bar2");
//    EXPECT_EQ(span.GetContext().trace_state()["foo"], "bar3");
//    EXPECT_EQ(span.GetContext().trace_state()["foo-_*/bar"], "bar4");
//}


// Dilapidated Tests:
//// I am not very sure how to make tests for httpformat_test as some functions output complex objects which cannot be
//// compared with some hard coded answers.
//TEST(HTTPTextFormatTest, DefaultConstructionNoCrash)
//{
//    HTTPTextFormat httptextformat = HTTPTextFormat();
//}
//
//TEST(HTTPTextFormatTest, ExtractNoCrush)
//{
//    HTTPTextFormat httptextformat = HTTPTextFormat();
//    Context ctx = Context();
//    std::map<char,int> carrier;
//    extract(getter, carrier, ctx);
//}
//
//TEST(HTTPTextFormatTest, InjectNoCrush)
//{
//    HTTPTextFormat httptextformat = HTTPTextFormat();
//    Context ctx = Context();
//    std::map<char,int> carrier = {{'1',1},{'2',2},{'3',3}};
//    inject(setter, carrier, ctx);
//}
//
//// In this test we want to see the after injecting a header's information into a context,
//// when we extract it it will give use back the same information for the header info.
//TEST(HTTPTextFormatTest, CompositeOperations)
//{
//    HTTPTextFormat httptextformat = HTTPTextFormat();
//    Context ctx = Context();
//    std::map<char,int> carrier = {{'1',1},{'2',2},{'3',3}};
//    inject(setter, carrier, ctx);
//    std::map<char,int> res;
//    extract(getter, res, ctx);
//    EXPECT_EQ(res, carrier);
//}