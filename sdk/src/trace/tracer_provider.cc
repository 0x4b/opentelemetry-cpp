#include "opentelemetry/sdk/trace/tracer_provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
TracerProvider::TracerProvider(std::shared_ptr<SpanProcessor> processor) noexcept
    : processor_{processor}, tracer_(new Tracer(std::move(processor)))
{}

opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> TracerProvider::GetTracer(
    nostd::string_view library_name,
    nostd::string_view library_version) noexcept
{
  return tracer_;
}

void TracerProvider::SetProcessor(std::shared_ptr<SpanProcessor> processor) noexcept
{
  processor_.store(processor);

  auto sdkTracer = static_cast<Tracer *>(tracer_.get());
  sdkTracer->SetProcessor(processor);
}

std::shared_ptr<SpanProcessor> TracerProvider::GetProcessor() const noexcept
{
  return processor_.load();
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
