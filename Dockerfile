# syntax=docker/dockerfile:1
FROM gcc:13-bookworm AS build
WORKDIR /app
COPY src ./src
COPY Makefile ./Makefile
RUN make app

FROM debian:bookworm-slim
WORKDIR /app
COPY --from=build /app/bin/diagnostic_session_manager ./diagnostic_session_manager
ENTRYPOINT ["./diagnostic_session_manager"]
