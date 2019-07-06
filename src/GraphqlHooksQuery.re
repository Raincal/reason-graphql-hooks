open GraphqlHooksTypes;

module Make = (Config: Config) => {
  [@bs.deriving abstract]
  type useQueryOptions = {
    [@bs.optional]
    useCache: bool,
    [@bs.optional]
    isManual: bool,
    [@bs.optional]
    variables: Js.Json.t,
    [@bs.optional]
    operationName: string,
    [@bs.optional]
    skipCache: bool,
    [@bs.optional]
    ssr: bool,
    [@bs.optional]
    fetchOptionsOverrides: Fetch.requestInit,
    [@bs.optional]
    updateData: (Config.t, Config.t) => Config.t,
  };

  [@bs.deriving abstract]
  type useQueryResponseJs = {
    loading: bool,
    cacheHit: bool,
    data: Js.Nullable.t(Js.Json.t),
    error: bool,
    graphQLErrors: Js.Nullable.t(graphqlErrors),
    fetchError: Js.Nullable.t(fetchError),
    httpError: Js.Nullable.t(httpError),
    refetch: useQueryOptions => Js.Promise.t(useQueryResponseJs),
  };

  [@bs.module "graphql-hooks"]
  external useQueryJs: (string, useQueryOptions) => useQueryResponseJs =
    "useQuery";

  let use =
      (
        ~useCache=?,
        ~isManual=?,
        ~variables=?,
        ~operationName=?,
        ~skipCache=?,
        ~ssr=?,
        ~updateData=?,
        (),
      ) => {
    let options =
      useQueryOptions(
        ~useCache?,
        ~isManual?,
        ~variables?,
        ~operationName?,
        ~skipCache?,
        ~ssr?,
        ~updateData?,
        (),
      );
    let state = useQueryJs(Config.query, options);

    let refetch = (~variables=?, ()) =>
      state->refetchGet(useQueryOptions(~variables?, ()));

    let useQueryResponseToRecord = (parse, state) => {
      let data = state->dataGet->Js.Nullable.toOption->Belt.Option.map(parse);
      let loading = state->loadingGet;
      let error = state->errorGet;
      let graphQLErrors = state->graphQLErrorsGet->Js.Nullable.toOption;
      let httpError = state->httpErrorGet->Js.Nullable.toOption;
      let fetchError = state->fetchErrorGet->Js.Nullable.toOption;
      let cacheHit = state->cacheHitGet;

      let result = {
        data,
        loading,
        error,
        graphQLErrors,
        httpError,
        fetchError,
      };

      let response =
        switch (result) {
        | {loading: true} => Loading
        | {data: Some(data)} => Data(data)
        | {error: true, graphQLErrors: Some(errors)} =>
          Error(GraphQLErrors(errors))
        | {error: true, httpError: Some(error)} => Error(HttpError(error))
        | {error: true, fetchError: Some(error)} =>
          Error(FetchError(error))
        | _ => NoData
        };

      {loading, data, cacheHit, response};
    };

    let state_record =
      React.useMemo2(
        () => state |> useQueryResponseToRecord(Config.parse),
        (state, Config.parse),
      );

    (state_record, refetch);
  };
};