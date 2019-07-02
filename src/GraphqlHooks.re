open GraphqlHooksTypes;

module Provider = GraphqlProvider;

module CreateQuery = (Config: Config) => Query.Make(Config);

module CreateMutation = (Config: Config) => Mutation.Make(Config);